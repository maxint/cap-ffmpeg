#ifndef __JNI_UTILS_H__
#define __JNI_UTILS_H__

#include <time.h>

#define	SAFE_MEM_FREE(MEM)		if (MEM) {free(MEM); MEM = NULL;}
#define	SAFE_MEM_DEL(MEM)		if (MEM) {delete MEM; MEM = NULL;}
#define	SAFE_MEMARR_DEL(MEM)	if (MEM) {delete[] MEM; MEM = NULL;}

#define	SAFE_LOCALREF_DEL(REF)	if (REF) {env->DeleteLocalRef(REF); REF = NULL;}
#define	SAFE_GLOBALREF_DEL(REF)	if (REF) {env->DeleteGlobalRef(REF); REF = NULL;}

#define	getThreadId()	        syscall(__NR_gettid)

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
