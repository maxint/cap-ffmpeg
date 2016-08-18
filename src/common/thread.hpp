#ifndef __JNI_THREAD_HPP__
#define __JNI_THREAD_HPP__

#include <pthread.h>

class mutex
{
public:
    mutex()     { pthread_mutex_init(&handle, NULL); }
    ~mutex()    { pthread_mutex_destroy(&handle); }

    void lock() { pthread_mutex_lock(&handle); }
    bool try_lock() { return pthread_mutex_trylock(&handle) == 0; }
    void unlock() { pthread_mutex_unlock(&handle); }

private:
    pthread_mutex_t handle;
};

template <class Mutex>
class lock_guard
{
public:
    typedef Mutex mutex_type;
    explicit lock_guard(mutex_type& m) : pm(m) { pm.lock(); }
    ~lock_guard() { pm.unlock(); }

private:
    mutex_type& pm; // exposition only
};

typedef lock_guard<mutex> auto_lock;

#endif // __JNI_THREAD_HPP__
