#include "node.h"
#include "uv.h"
#include "sync.h"
#include "thread.h"
#include "queue.h"
  
void init(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD( exports, "thread", Thread::Thread );
  NODE_SET_METHOD( exports, "sync", Sync::Sync );
  NODE_SET_METHOD( exports, "queue", Queue::Queue);
}

NODE_MODULE(EzitoAsync, init)

