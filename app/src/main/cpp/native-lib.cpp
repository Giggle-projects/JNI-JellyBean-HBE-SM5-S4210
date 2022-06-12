#include <jni.h>
#include <string>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

std::string errorMessage = "ERROR";
std::string successMessage = "SUCCEED";

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_ledControl(
        JNIEnv *env,
        jobject /* this */,
        jint bitCount
) {
    int fd = open("/dev/fpga_led", O_WRONLY);
    if (fd == -1) {
        return env->NewStringUTF(errorMessage.c_str());
    }

    unsigned char val = (((bitCount & 0x1) << 7) | ((bitCount & 0x2) << 5) | ((bitCount & 0x4) << 3) | \
            ((bitCount & 0x8) << 1) | ((bitCount & 0x10) >> 1) | ((bitCount & 0x20) >> 3) | \
            ((bitCount & 0x40) >> 5) | ((bitCount & 0x80) >> 7));
    write(fd, &val, sizeof(val));

    close(fd);
    return env->NewStringUTF(successMessage.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_piezoControl(
        JNIEnv *env,
        jobject /* this */,
        jint num) {
    int fd = open("/dev/fpga_piezo", O_WRONLY);
    if (fd == -1) {
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

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_segmentControl(
        JNIEnv *env,
        jobject /* this */,
        jint data) {

    int fd = open("/dev/fpga_segment", O_RDWR | O_SYNC);
    if (fd == -1) {
        return env->NewStringUTF(errorMessage.c_str());
    }

    char buf[7];
    sprintf(buf, "%06d", data);
    write(fd, buf, 6);

    close(fd);
    return env->NewStringUTF(successMessage.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_lcdInit(
        JNIEnv *env,
        jobject obj) {
    int fd = open("/dev/fpga_textlcd", O_WRONLY);
    if (fd == -1) {
        std::string hello = "ERROR";
        return env->NewStringUTF(hello.c_str());
    }
    ioctl(fd, 3);
    close(fd);
    std::string hello = "LCD";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_lcdPrint(JNIEnv *env, jobject obj, jstring msg) {
    const char *str;

    int fd = open("/dev/fpga_textlcd", O_WRONLY);
    if (fd == -1) {
        std::string hello = "ERROR";
        return env->NewStringUTF(hello.c_str());
    }
    str = env->GetStringUTFChars(msg, 0);
    ioctl(fd, 5);
    write(fd, str, strlen(str));
    std::string hello = "LCD";
    return env->NewStringUTF(hello.c_str());
}