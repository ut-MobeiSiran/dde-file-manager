#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------
#system($$PWD/../vendor/prebuild)
#include($$PWD/../vendor/vendor.pri)

INCLUDEPATH += $$PWD \
               $$PWD/../../../../../src/dde-file-manager-plugins/pluginPreview/dde-music-preview-plugin/

HEADERS += \
    $$PWD/interfaces/tst_all_interfaces.h

SOURCES += \
    $$PWD/test-main.cpp \
    $$PWD/ut_musicmessageview.cpp \
    $$PWD/ut_musicpreview.cpp \
    $$PWD/ut_musicpreviewplugin.cpp
