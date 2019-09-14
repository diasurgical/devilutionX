LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS :=

LOCAL_MODULE    := libsmacker
LOCAL_SRC_FILES :=\
		smk_bitstream.c \
		smk_hufftree.c \
		smacker.c \

include $(BUILD_STATIC_LIBRARY)
