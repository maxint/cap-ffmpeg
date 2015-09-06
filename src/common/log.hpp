#ifndef __JNI_LOG_HPP__
#define __JNI_LOG_HPP__

#include "utils.hpp"

#if defined(__ANDROID__) || defined(ANDROID)
#  include <android/log.h>
#  ifndef LOG_TAG
#    define  LOG_TAG    "JNI"
#  endif
#  define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG, ##__VA_ARGS__)
#  define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,  LOG_TAG, ##__VA_ARGS__)
#  define LOGI(...) __android_log_print(ANDROID_LOG_INFO,   LOG_TAG, ##__VA_ARGS__)
#  define LOGW(...) __android_log_print(ANDROID_LOG_WARN,   LOG_TAG, ##__VA_ARGS__)
#  define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,  LOG_TAG, ##__VA_ARGS__)
#else
#  define _LOG_(OUT, L, fmt, ...) fprintf(OUT, "%s/%s: " fmt ", in %s (%d)\n", LOG_TAG, L, ##__VA_ARGS__, __FILE__, __LINE__)
#  define LOGV(fmt, ...) _LOG_(stdout, "V", fmt, ##__VA_ARGS__)
#  define LOGD(fmt, ...) _LOG_(stdout, "D", fmt, ##__VA_ARGS__)
#  define LOGI(fmt, ...) _LOG_(stdout, "I", fmt, ##__VA_ARGS__)
#  define LOGW(fmt, ...) _LOG_(stderr, "W", fmt, ##__VA_ARGS__)
#  define LOGE(fmt, ...) _LOG_(stderr, "E", fmt, ##__VA_ARGS__)
#endif

#ifndef NO_FUNC_LOG
#   define ENTER_FUNCTION	LOGV("<--- %s:%s", __FILE__, __FUNCTION__)
#   define EXIT_FUNCTION	LOGV("---> %s:%s", __FILE__, __FUNCTION__)
#	define QLOG				LOGD("%s(%d):%s", __FILE__, __LINE__, __FUNCTION__)
#else
#   define ENTER_FUNCTION
#   define EXIT_FUNCTION
#	define QLOG
#endif	//end of NO_JNI_LOG

#define JNI_TIME		//if JNI_TIME be defined, function cost time will be print.
#ifdef JNI_TIME
#   define LOGTIME          LOGD
#   define BEGIN_TIME       { int64_t t0 = getTime()
#   define END_TIME(FUN)    LOGTIME("%s:%s cost time %.3fms", __FILE__, FUN, (getTime()-t0) / 1000.0);}
#else
#   define BEGIN_TIME
#   define END_TIME(FUN)
#endif	//end of JNI_TIME

#endif // end of __JNI_LOG_HPP__
