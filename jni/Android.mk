LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := nonfree
LOCAL_SRC_FILES := libnonfree.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := opencv_java_prebuilt
LOCAL_SRC_FILES := libopencv_java.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
include E:\Android_openCV\OpenCV-2.4.7-android-sdk\sdk\native\jni\OpenCV.mk
LOCAL_MODULE    := Sift
LOCAL_CFLAGS    := -Werror -O3 -ffast-math
LOCAL_LDLIBS    += -llog -ldl 
LOCAL_SHARED_LIBRARIES := nonfree opencv_java_prebuilt
LOCAL_SRC_FILES := extract_fea.cpp
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
include E:\Android_openCV\OpenCV-2.4.7-android-sdk\sdk\native\jni\OpenCV.mk
LOCAL_MODULE    := cluster
LOCAL_CFLAGS    := -Werror -O3 -ffast-math
LOCAL_LDLIBS    += -llog -ldl 
LOCAL_SRC_FILES := cluster.cpp
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
include E:\Android_openCV\OpenCV-2.4.7-android-sdk\sdk\native\jni\OpenCV.mk
LOCAL_MODULE    := freCount
LOCAL_CFLAGS    := -Werror -O3 -ffast-math
LOCAL_LDLIBS    += -llog -ldl 
LOCAL_SRC_FILES := frequencyCount.cpp
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
include E:\Android_openCV\OpenCV-2.4.7-android-sdk\sdk\native\jni\OpenCV.mk
LOCAL_MODULE    := ldaApply
LOCAL_CFLAGS    := -Werror -O3 -ffast-math
LOCAL_LDLIBS    += -llog -ldl 
LOCAL_SRC_FILES := ldaApply.cpp
LOCAL_SRC_FILES += ./lda/document.cc
LOCAL_SRC_FILES += ./lda/file_access.cc
LOCAL_SRC_FILES += ./lda/lda.cc
include $(BUILD_SHARED_LIBRARY)