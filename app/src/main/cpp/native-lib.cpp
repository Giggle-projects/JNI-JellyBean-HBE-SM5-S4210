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
Java_com_example_myapplication_GlobalNative_ledControl(
        JNIEnv *env,
        jclass clazz,
        jint bitCount
) {
    int fd = open("/dev/fpga_led", O_WRONLY);
    if (fd == -1) {
        return env->NewStringUTF(errorMessage.c_str());
    }

    unsigned char val = (((bitCount & 0x1) << 7) | ((bitCount & 0x2) << 5) |
                         ((bitCount & 0x4) << 3) | \
            ((bitCount & 0x8) << 1) | ((bitCount & 0x10) >> 1) | ((bitCount & 0x20) >> 3) | \
            ((bitCount & 0x40) >> 5) | ((bitCount & 0x80) >> 7));
    write(fd, &val, sizeof(val));

    close(fd);
    return env->NewStringUTF(successMessage.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_GlobalNative_segmentControl(
        JNIEnv *env,
        jclass clazz,
        jint data
) {
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
Java_com_example_myapplication_GlobalNative_lcdClear(
        JNIEnv *env,
        jclass clazz
) {
    int fd = open("/dev/fpga_textlcd", O_WRONLY | O_NDELAY);
    if (fd == -1) {
        return env->NewStringUTF(errorMessage.c_str());
    }
    ioctl(fd, 3);
    close(fd);
    return env->NewStringUTF(successMessage.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_GlobalNative_lcdPrint(
        JNIEnv *env,
        jclass clazz,
        jint lineIndex,
        jstring msg
) {
    int fd = open("/dev/fpga_textlcd", O_WRONLY | O_NDELAY);
    if (fd == -1) {
        return env->NewStringUTF(errorMessage.c_str());
    }
    const char *str = env->GetStringUTFChars(msg, 0);
    switch (lineIndex) {
        case 0:
            ioctl(fd, 5);
            break;
        case 1 :
            ioctl(fd, 6);
            break;
        default :
            std::string msg = "line index must be under 2";
            return env->NewStringUTF(msg.c_str());
    }
    write(fd, str, strlen(str));
    close(fd);
    return env->NewStringUTF(successMessage.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_piezoControl(
        JNIEnv* env,
        jobject clazz,
        jint num
) {
    int fd = open("/dev/fpga_piezo", O_WRONLY);
    if(fd == -1 ) {
        return env->NewStringUTF(errorMessage.c_str());
    }

    unsigned char val = num - '0';
    write(fd, &val, 1);

    close(fd);
    return env->NewStringUTF(successMessage.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_GlobalNative_dotMatrixControl(
        JNIEnv *env,
        jclass clazz,
        jstring data
) {
    const char *pStr = env->GetStringUTFChars(data, 0);
    int len = env->GetStringLength(data);

    int fd = open("/dev/fpga_dotmatrix", O_RDWR | O_SYNC);
    if (fd == -1) {
        return env->NewStringUTF(errorMessage.c_str());
    }

    write(fd, pStr, len);
    close(fd);
    return env->NewStringUTF(successMessage.c_str());
}
