#-------------------------------------------------
#
# Project created by QtCreator 2011-01-17T23:02:32
#
#-------------------------------------------------

QT       += gui network
CONFIG += uitools

TARGET = DoNothing
TEMPLATE = lib

SOURCES += donothing.cpp \
    settingsdialog.cpp

HEADERS += donothing.h \
    settingsdialog.h

include(../../qtcreatorplugin.pri)
#DESTDIR = $$IDE_PLUGIN_PATH/VCreateLogic
PROVIDER = VCreateLogic
include(../../plugins/coreplugin/coreplugin.pri)
OTHER_FILES += DoNothing.pluginspec

FORMS += \
    settingsdialog.ui
