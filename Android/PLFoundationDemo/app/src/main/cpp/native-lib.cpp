#include <jni.h>
#include <string>
#include <iostream>
#include <pthread.h>

using namespace std;

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_plfoundationdemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    pthread_spin_init

    return env->NewStringUTF(hello.c_str());
}
