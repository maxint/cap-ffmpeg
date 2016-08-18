#ifndef __JNI_UTILS_H__
#define __JNI_UTILS_H__

#include <time.h>
#include <stdint.h>

// return microsecond
static inline int64_t getTime(void) {
#ifdef __linux__
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    // seconds and nanoseconds
    return now.tv_sec*1000000 + now.tv_nsec/1000;
#else
    return 0;
#endif
}

#undef max
#undef min
template<typename T> T max(T a, T b) { return a > b ? a : b; }
template<typename T> T min(T a, T b) { return a < b ? a : b; }


#endif	//end of __JNI_UTILS_H__
