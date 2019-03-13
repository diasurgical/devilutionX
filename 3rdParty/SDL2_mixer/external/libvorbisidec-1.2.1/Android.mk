LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vorbisidec

LOCAL_C_INCLUDES :=

LOCAL_CFLAGS :=

ifeq ($(TARGET_ARCH),arm)
    LOCAL_CFLAGS += -D_ARM_ASSEM_
endif

LOCAL_SRC_FILES += \
    mdct.c.arm \
    block.c \
    window.c.arm \
    synthesis.c \
    info.c \
    floor1.c.arm \
    floor0.c.arm \
    vorbisfile.c.arm \
    res012.c \
    mapping0.c \
    registry.c \
    codebook.c \
    sharedbook.c.arm \

LOCAL_SHARED_LIBRARIES := ogg

include $(BUILD_STATIC_LIBRARY)
