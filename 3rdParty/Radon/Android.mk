LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS :=

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Radon/include

LOCAL_MODULE    := libradon
LOCAL_SRC_FILES :=\
	  $(LOCAL_PATH)/Radon/source/File.cpp \
	  $(LOCAL_PATH)/Radon/source/Key.cpp \
	  $(LOCAL_PATH)/Radon/source/Named.cpp \
	  $(LOCAL_PATH)/Radon/source/Section.cpp \

include $(BUILD_STATIC_LIBRARY)
