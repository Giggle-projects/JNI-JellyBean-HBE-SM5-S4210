#include <jni.h>
#include <string>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_myapplication_MainActivity_stringFromJNI(
//        JNIEnv* env,
//        jobject /* this */) {
//
//
//    std::string hello = "Hello from C++";
//
//    return env->NewStringUTF(hello.c_str());
//}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_ledControl(
        JNIEnv* env,
        jobject /* this */) {
    int fd = open("/dev/fpga_led", O_WRONLY);
    if(fd == -1 ) {
        std::string hello = "ERROR";
        return env->NewStringUTF(hello.c_str());
    }

    unsigned char val;
    val = '3'-'0';
    write(fd, &val, sizeof(val));

    close(fd);
    std::string hello = "LED";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_piezoControl(JNIEnv* env, jobject /* this */, jint num) {
    int fd = open("/dev/fpga_piezo", O_WRONLY);
    if(fd == -1 ) {
        std::string hello = "ERROR";
        return env->NewStringUTF(hello.c_str());
    }

    unsigned char val;
    val = num;
    write(fd, &val, sizeof(val));

    close(fd);
    std::string hello = "LED";
    return env->NewStringUTF(hello.c_str());
}
