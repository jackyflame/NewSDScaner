# Test that LOCAL_CPPFLAGS only works for C++ sources
#
LOCAL_PATH:= $(call my-dir)

 
include $(CLEAR_VARS)
LOCAL_MODULE    := engine
LOCAL_C_INCLUDES := $(LOCAL_PATH)/engine
LOCAL_CPPFLAGS := -UBANANA -DBANANA=300 
LOCAL_SRC_FILES := \
	engine/StdAfx.cpp \
	engine/ImageBase.cpp \
	engine/RecogCore.cpp \
	engine/RecogMQDF.cpp \
	engine/LineRecogPrint.cpp \
	engine/Engine.cpp \
	engine/RecogEngine.cpp
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := testEngine
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CPPFLAGS := -UBANANA -DBANANA=300
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl -lm -llog -ljnigraphics
LOCAL_SRC_FILES := \
	zinterface.cpp
LOCAL_STATIC_LIBRARIES := engine

include $(BUILD_SHARED_LIBRARY)

