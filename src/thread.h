

#ifndef EZITO_THREAD_HEAD
#define EZITO_THREAD_HEAD

#include "node.h"
#include "uv.h"


namespace Thread {
    typedef struct { uv_async_t async; uv_mutex_t mutex; uv_thread_t thread; uv_work_t  request;v8::Persistent<v8::Function> callback; } AsyncWorker;
    
    inline uv_loop_t* Uv_Current_Loop() {
        #if NODE_MAJOR_VERSION >= 10 ||  NODE_MAJOR_VERSION == 9 && NODE_MINOR_VERSION >= 3 || NODE_MAJOR_VERSION == 8 && NODE_MINOR_VERSION >= 10
            return node::GetCurrentEventLoop(v8::Isolate::GetCurrent());
        #else
            return uv_default_loop();
        #endif
    };

    void Thread_Async_Cb(uv_async_t* async, int status) {
        v8::Isolate * isolate = v8::Isolate::GetCurrent(); 
        v8::HandleScope handleScope(isolate); 
        Thread::AsyncWorker * async_worker = static_cast<Thread::AsyncWorker *>(async->data);
        v8::Local<v8::Function> fn = v8::Local<v8::Function>::New(isolate, async_worker->callback);
        fn -> Call( isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(),0,0 );
        uv_close((uv_handle_t*) async, NULL);
    }

    void Thread_Cb( void*data){
        AsyncWorker * async_worker = static_cast<AsyncWorker *> (data);
        uv_mutex_lock(&async_worker->mutex);
        uv_async_send(&async_worker->async);
        uv_mutex_unlock(&async_worker->mutex);
    };

    void Thread(const v8::FunctionCallbackInfo<v8::Value>& argumants) {
        assert(uv_available_parallelism()); 
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = isolate -> GetCurrentContext();
        if(! argumants[0]->IsAsyncFunction() && ! argumants[0] -> IsFunction()){
            return;
        }

        Thread::AsyncWorker * async_worker = new Thread::AsyncWorker(); 
        async_worker -> callback.Reset(isolate , v8::Local<v8::Function>::Cast(argumants[0]));
        async_worker -> async.data = async_worker;
        uv_mutex_init(& async_worker -> mutex);
        uv_async_init( Sync::Uv_Current_Loop() , & async_worker -> async , uv_async_cb(Thread::Thread_Async_Cb));
        uv_thread_create(& async_worker -> thread, uv_thread_cb( Thread::Thread_Cb), async_worker);
        uv_thread_join(& async_worker -> thread);
    };
};

#endif
