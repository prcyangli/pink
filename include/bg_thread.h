#ifndef BG_THREAD_H_
#define BG_THREAD_H_

#include <deque>
#include <atomic>
#include "pink_thread.h"

namespace pink {

class BGThread : public Thread {
  public:
    BGThread() :
      Thread::Thread(), exit_(false), running_(false) {
        pthread_mutex_init(&mu_, NULL);
        pthread_cond_init(&signal_, NULL);
      }

    virtual ~BGThread() {
      exit_ = true;
      pthread_cond_signal(&signal_);
      pthread_join(thread_id(), NULL);
      pthread_cond_destroy(&signal_);
      pthread_mutex_destroy(&mu_);
    }
    bool is_running() {
      return running_;
    }

    void Schedule(void (*function)(void*), void* arg);

  private:
    struct BGItem {
      void (*function)(void*);
      void* arg;
      BGItem(void (*_function)(void*), void* _arg)
        : function(_function), arg(_arg) {}
    };
    typedef std::deque<BGItem> BGQueue;
    pthread_mutex_t mu_;
    pthread_cond_t signal_;
    BGQueue queue_;
    std::atomic<bool> exit_;
    std::atomic<bool> running_;
    virtual void *ThreadMain(); 
};

}

#endif