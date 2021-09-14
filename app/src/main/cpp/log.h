//
// Created by Administrator on 2021/9/10.
//

#ifndef CHECKNATIVEHOOK_LOG_H
#define CHECKNATIVEHOOK_LOG_H

#include <android/log.h>
#define LOG_TAG        "CheckHook_TAG"
#define LOGD(...)      __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...)      __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif //CHECKNATIVEHOOK_LOG_H
