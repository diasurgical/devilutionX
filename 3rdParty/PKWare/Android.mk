LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS :=

LOCAL_MODULE    := pkware
LOCAL_SRC_FILES :=\
		  	$(LOCAL_PATH)/explode.cpp \
  			$(LOCAL_PATH)/implode.cpp \

include $(BUILD_STATIC_LIBRARY)
