

#ifndef EZITO_QUEUE_HEAD
#define EZITO_QUEUE_HEAD

#include "node.h"
#include "uv.h"


namespace Queue {
    typedef struct {   uv_work_t  request;v8::Persistent<v8::Function> callback; } AsyncWorker;
    
    inline uv_loop_t* Uv_Current_Loop() {
        #if NODE_MAJOR_VERSION >= 10 ||  NODE_MAJOR_VERSION == 9 && NODE_MINOR_VERSION >= 3 || NODE_MAJOR_VERSION == 8 && NODE_MINOR_VERSION >= 10
            return node::GetCurrentEventLoop(v8::Isolate::GetCurrent());
        #else
            return uv_default_loop();
        #endif
    };

    void Async_Queue_Work(uv_work_t * request) {
    }

    void Async_Queue_Work_After(uv_work_t * request){
        Queue::AsyncWorker * async_worker = static_cast<Queue::AsyncWorker *> ( request->data);
        v8::Isolate * isolate = v8::Isolate::GetCurrent(); 
        v8::HandleScope handleScope(isolate);  
        v8::Local<v8::Function> fn = v8::Local<v8::Function>::New(isolate, async_worker->callback);
        fn -> Call( isolate->GetCurrentContext(), isolate->GetCurrentContext()->Global(),0,0 );
    };

    void Queue(const v8::FunctionCallbackInfo<v8::Value>& argumants) {
        assert(uv_available_parallelism()); 
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = isolate -> GetCurrentContext();
        if(! argumants[0]->IsAsyncFunction() && ! argumants[0] -> IsFunction()){
            return;
        }

        Queue::AsyncWorker * async_worker = new Queue::AsyncWorker(); 
        async_worker -> callback.Reset(isolate , v8::Local<v8::Function>::Cast(argumants[0]));
        async_worker -> request.data = async_worker;
        uv_queue_work( Queue::Uv_Current_Loop() , &async_worker->request, uv_work_cb(Queue::Async_Queue_Work),uv_after_work_cb(Queue::Async_Queue_Work_After));

    };
};

#endif
