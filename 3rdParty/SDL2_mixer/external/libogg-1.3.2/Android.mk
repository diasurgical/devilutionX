LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ogg

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/android

LOCAL_CFLAGS :=

LOCAL_SRC_FILES += \
    src/framing.c \
    src/bitwise.c

LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)
