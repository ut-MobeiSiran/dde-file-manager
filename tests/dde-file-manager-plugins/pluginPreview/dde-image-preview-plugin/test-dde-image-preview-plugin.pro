#-------------------------------------------------
#
# Project created by QtCreator 2017-03-31T16:00:06
#
#-------------------------------------------------

QT       += widgets   quick

TARGET = test-dde-image-preview-plugin
TEMPLATE = app
CONFIG += c++11 console link_pkgconfig

PKGCONFIG += dtkwidget dtkgui

include(../../../../src/common/common.pri)

LIBS += -L$$OUT_PWD/../../../../src/dde-file-manager-lib -ldde-file-manager

DISTFILES += dde-image-preview-plugin.json

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0

SOURCES += \
    $$PWD/../../../../src/dde-file-manager-plugins/pluginPreview/dde-image-preview-plugin/imageview.cpp \
    $$PWD/../../../../src/dde-file-manager-plugins/pluginPreview/dde-image-preview-plugin/imagepreview.cpp \
    $$PWD/../../../../src/dde-file-manager-plugins/pluginPreview/dde-image-preview-plugin/imagepreviewplugin.cpp

HEADERS += \
    $$PWD/../../../../src/dde-file-manager-plugins/pluginPreview/dde-image-preview-plugin/imageview.h \
    $$PWD/../../../../src/dde-file-manager-plugins/pluginPreview/dde-image-preview-plugin/imagepreview.h \
    $$PWD/../../../../src/dde-file-manager-plugins/pluginPreview/dde-image-preview-plugin/imagepreviewplugin.h

include(../../../../3rdparty/googletest/gtest_dependency.pri)
include(../../../../3rdparty/cpp-stub/stub.pri)
include(tests/test.pri)

!CONFIG(DISABLE_TSAN_TOOL) {
    #DEFINES += TSAN_THREAD #互斥
    DEFINES += ENABLE_TSAN_TOOL
    contains(DEFINES, TSAN_THREAD){
       QMAKE_CXXFLAGS+="-fsanitize=thread"
       QMAKE_CFLAGS+="-fsanitize=thread"
       QMAKE_LFLAGS+="-fsanitize=thread"
    } else {
       QMAKE_CXXFLAGS+="-fsanitize=undefined,address,leak -fno-omit-frame-pointer"
       QMAKE_CFLAGS+="-fsanitize=undefined,address,leak -fno-omit-frame-pointer"
       QMAKE_LFLAGS+="-fsanitize=undefined,address,leak -fno-omit-frame-pointer"
    }
}

