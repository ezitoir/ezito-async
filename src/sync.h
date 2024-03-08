

#ifndef EZITO_SYNC_HEAD
#define EZITO_SYNC_HEAD

#include "node.h"
#include "uv.h"


namespace Sync {
    typedef struct {
        uv_async_t async;
        uv_mutex_t mutex;
        uv_thread_t thread; 
        uv_work_t  request; 
        v8::Persistent<v8::Value> result; 
        v8::Persistent<v8::Function> callback; 
    } AsyncWorker;

    inline uv_loop_t* Uv_Current_Loop() {
        #if NODE_MAJOR_VERSION >= 10 ||  NODE_MAJOR_VERSION == 9 && NODE_MINOR_VERSION >= 3 || NODE_MAJOR_VERSION == 8 && NODE_MINOR_VERSION >= 10
            return node::GetCurrentEventLoop(v8::Isolate::GetCurrent());
        #else
            return uv_default_loop();
        #endif
    };

    void Call_Back_Function_Result(const v8::FunctionCallbackInfo<v8::Value>& argumants){
        Sync::AsyncWorker * async_worker = static_cast<Sync::AsyncWorker *>(argumants.Data().As<v8::External>()->Value());
        async_worker -> result.Reset( v8::Isolate::GetCurrent() , argumants[0]);
        argumants.GetReturnValue().Set(v8::Null( v8::Isolate::GetCurrent()));
    };

    void Sync_Cb(uv_async_t* async, int status) {
        v8::Isolate * isolate = v8::Isolate::GetCurrent(); 
        v8::HandleScope handleScope(isolate); 
        Sync::AsyncWorker * async_worker = static_cast<Sync::AsyncWorker *>(async->data);
        v8::Local<v8::External> ext = v8::External::New( isolate , async_worker);
        v8::Local<v8::FunctionTemplate> result_set_function_template = v8::FunctionTemplate::New(
            isolate,
            Call_Back_Function_Result,
            ext
        );
        v8::Local<v8::Function> result_set_function = result_set_function_template->GetFunction(isolate->GetCurrentContext()).ToLocalChecked();
        v8::Local<v8::Value>argc[] = { result_set_function };
        v8::Local<v8::Function> fn = v8::Local<v8::Function>::New(isolate, async_worker->callback);
        fn -> Call( isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global() , 1 , argc);
        argc -> Clear();
        uv_close((uv_handle_t*) async, NULL);
    }

    void Sync_Thread_Cb( void*data){
        AsyncWorker * async_worker = static_cast<AsyncWorker *> (data);
        uv_mutex_lock(&async_worker->mutex);
        uv_async_send(&async_worker->async);
        uv_mutex_unlock(&async_worker->mutex);
    };

    void Sync(const v8::FunctionCallbackInfo<v8::Value>& argumants) {
        assert(uv_available_parallelism());
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = isolate -> GetCurrentContext();
        if(! argumants[0]->IsAsyncFunction() && ! argumants[0] -> IsFunction()){
            return;
        }

        Sync::AsyncWorker * async_worker = new Sync::AsyncWorker();  
        async_worker -> callback.Reset(isolate , v8::Local<v8::Function>::Cast(argumants[0]));
        async_worker -> async.data = async_worker;
        uv_mutex_init( &async_worker -> mutex);
        uv_async_init( Sync::Uv_Current_Loop() , &async_worker->async , uv_async_cb(Sync::Sync_Cb));
        uv_thread_create(&async_worker->thread, uv_thread_cb(Sync::Sync_Thread_Cb), async_worker);
        uv_run( Sync::Uv_Current_Loop() , UV_RUN_DEFAULT);
        
        argumants.GetReturnValue().Set(v8::Local<v8::Value>::New(isolate,async_worker->result));
    };
};

#endif
