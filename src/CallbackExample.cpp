#include <jni.h>
#include <iostream>
#include <map>
#include <mutex>
#include <cstring>
#include "CallbackExample.h"

struct CallbackData {
    char name[20];
    int intValue;
    double doubleValue;
    long longValue;
};

// 全局引用Java虚拟机
JavaVM *globalJvm = nullptr;

class Business {
public:
    Business(JNIEnv *env, jobject callback) {
        this->env = env;
        this->callback = env->NewGlobalRef(callback);
    }

    ~Business() {
        env->DeleteGlobalRef(callback);
    }

    void start() {
    // 模拟业务处理并触发回调
    CallbackData data;
    strcpy(data.name, "TestName");
    data.intValue = 42;
    data.doubleValue = 3.14;
    data.longValue = 123456789L;
    
    triggerCallback(data); // 调用回调
    }

private:
    JNIEnv *env;
    jobject callback;
    std::mutex callbackMutex;



void triggerCallback(const CallbackData &data) {
    std::lock_guard<std::mutex> lock(callbackMutex);

    JNIEnv* env;
    int getEnvStat = globalJvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    bool attached = false;
    
    if (getEnvStat == JNI_EDETACHED) {
        if (globalJvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != 0) {
            std::cerr << "Failed to attach current thread" << std::endl;
            return;
        }
        attached = true;
    }else if (getEnvStat == JNI_OK) {
        // Already attached
    } else {
        std::cerr << "Failed to get the environment" << std::endl;
        return;
    }

    // 获取回调类和方法ID
    jclass callbackClass = env->GetObjectClass(callback);
    if (callbackClass == nullptr) {
        std::cerr << "Failed to find callback class" << std::endl;
        if (attached) {
            globalJvm->DetachCurrentThread();
        }
        return;
    }

    jmethodID onCallbackMethod = env->GetMethodID(callbackClass, "onCallback", "(LCallbackExample$CallbackData;)V");
    if (onCallbackMethod == nullptr) {
        std::cerr << "Failed to find onCallback method" << std::endl;
        if (attached) {
            globalJvm->DetachCurrentThread();
        }
        return;
    }

    jclass dataClass = env->FindClass("CallbackExample$CallbackData");
    if (dataClass == nullptr) {
        std::cerr << "Failed to find CallbackData class" << std::endl;
        if (attached) {
            globalJvm->DetachCurrentThread();
        }
        return;
    }

    jmethodID dataConstructor = env->GetMethodID(dataClass, "<init>", "()V");
    if (dataConstructor == nullptr) {
        std::cerr << "Failed to find CallbackData constructor" << std::endl;
        if (attached) {
            globalJvm->DetachCurrentThread();
        }
        return;
    }

    jobject dataObject = env->NewObject(dataClass, dataConstructor);
    if (dataObject == nullptr) {
        std::cerr << "Failed to create CallbackData object" << std::endl;
        if (attached) {
            globalJvm->DetachCurrentThread();
        }
        return;
    }

    jmethodID setNameMethod = env->GetMethodID(dataClass, "setName", "(Ljava/lang/String;)V");
    jmethodID setIntValueMethod = env->GetMethodID(dataClass, "setIntValue", "(I)V");
    jmethodID setDoubleValueMethod = env->GetMethodID(dataClass, "setDoubleValue", "(D)V");
    jmethodID setLongValueMethod = env->GetMethodID(dataClass, "setLongValue", "(J)V");

    jstring name = env->NewStringUTF(data.name);
    env->CallVoidMethod(dataObject, setNameMethod, name);
    env->CallVoidMethod(dataObject, setIntValueMethod, data.intValue);
    env->CallVoidMethod(dataObject, setDoubleValueMethod, data.doubleValue);
    env->CallVoidMethod(dataObject, setLongValueMethod, data.longValue);

    // 调用 Java 回调
    env->CallVoidMethod(callback, onCallbackMethod, dataObject);

    // 释放局部引用
    env->DeleteLocalRef(name);
    env->DeleteLocalRef(dataObject);
    env->DeleteLocalRef(callbackClass);
    env->DeleteLocalRef(dataClass);

    if (attached) {
        globalJvm->DetachCurrentThread();
    }
}


};

std::map<int, Business*> businessMap;
std::mutex businessMapMutex;

extern "C" {

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    std::cout <<  "call JNI_OnLoad" << std::endl;
    globalJvm = vm;

    // 获取 JNI 环境
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        std::cerr << "JNI 版本不支持" << std::endl;
        return JNI_ERR; // JNI 版本不支持
    }

    // 在这里可以进行其他的初始化工作，比如注册本地方法

    return JNI_VERSION_1_6; // 返回 JNI 版本
}

JNIEXPORT void JNICALL Java_CallbackExample_registerCallback(JNIEnv *env, jobject obj, jint id, jobject callback) {
    std::lock_guard<std::mutex> lock(businessMapMutex);
    if (businessMap.find(id) == businessMap.end()) {
        businessMap[id] = new Business(env, callback);
    } else {
        std::cerr << "Callback for id " << id << " is already registered." << std::endl;
    }
}

JNIEXPORT void JNICALL Java_CallbackExample_startBusiness(JNIEnv *env, jobject obj, jint id) {
    std::lock_guard<std::mutex> lock(businessMapMutex);
    if (businessMap.find(id) != businessMap.end()) {
        businessMap[id]->start();
    } else {
        std::cerr << "No business registered for id " << id << std::endl;
    }
}

}
