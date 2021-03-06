#-------------------------------------------------
#
# Project created by QtCreator 2019-03-20T10:39:38
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = TestDataStorageDirManager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include("../../Common/Common.pri")
include("../../Common/googletest.pri")
include("../../Common/runtest.pri")

SOURCES += \
        TestDataStorageDirManager.cpp \ 
    main.cpp \
    ../../../Utility/DataStorage/DataStorageDirManager.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Components/TimeManager/TimeManagerInterface.cpp \
    ../../../Components/System/SystemManagerInterface.cpp \
    ../../../Components/LightManager/LightManagerInterface.cpp \
    ../../../Components/Alarm/AlarmSourceManager.cpp \
    ../../../Utility/DataStorage/DataStorageDirManagerInterface.cpp \
    ../../../Components/Alarm/AlarmInterface.cpp \
    ../../../Components/Alarm/AlarmParamIFace.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    TestDataStorageDirManager.h \
    ../../../Utility/DataStorage/DataStorageDirManager.h \
    ../../../Components/PatientData/RescueDataDefine.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Components/TimeManager/TimeManagerInterface.h \
    ../../../Components/ParamManager/ParamDefine.h \
    ../../../Components/System/SystemManagerInterface.h \
    ../../../Components/LightManager/LightManagerInterface.h \
    ../../../Components/Alarm/AlarmSourceManager.h \
    ../../../Utility/DataStorage/DataStorageDirManagerInterface.h \
    ../../../Components/Alarm/AlarmInterface.h \
    MockTimeManager.h \
    ../MockLightManager.h \
    ../../../Components/Alarm/AlarmParamIFace.h \
    MockSystemAlarm.h

INCLUDEPATH += \
    ../../../Utility/DataStorage \
    ../../../Components/PatientData \
    ../../../Utility/Config \
    ../../../Components/TimeManager \
    ../../../Components/ParamManager \
    ../../../Components/System \
    ../../../Components/LightManager  \
    ../../../Components/Alarm \
