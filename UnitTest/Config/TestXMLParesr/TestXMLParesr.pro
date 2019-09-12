QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app


include("../../Common/Common.pri")
include("../../Common/googletest.pri")
include("../../Common/runtest.pri")


SOURCES +=  \
    $$PROJECT_BASE/Utility/Config/XmlParser.cpp \
    $$PROJECT_BASE/Utility/ErrorLog/ErrorLogItem.cpp \
    $$PROJECT_BASE/Utility/ErrorLog/ErrorLogInterface.cpp \
    $$PROJECT_BASE/Utility/LanguageManager/LanguageManager.cpp \
    $$PROJECT_BASE/Utility/Config/Config.cpp \
    $$PROJECT_BASE/Utility/Config/IConfig.cpp \
    main.cpp \
    TestXmlParser.cpp

HEADERS += MockErrorLog.h \
    $$PROJECT_BASE/Utility/Config/XmlParser.h \
    $$PROJECT_BASE/Utility/ErrorLog/ErrorLogItem.h   \
    $$PROJECT_BASE/Utility/ErrorLog/ErrorLogInterface.h \
    $$PROJECT_BASE/Utility/LanguageManager/LanguageManager.h \
    $$PROJECT_BASE/Utility/Config/Config.h \
    $$PROJECT_BASE/Utility/Config/IConfig.h \
    TestXmlParser.h

INCLUDEPATH += \
    $$PROJECT_BASE/Utility/Config \
    $$PROJECT_BASE/Utility/ErrorLog \
    $$PROJECT_BASE/Utility/Config \
    $$PROJECT_BASE/Utility/LanguageManager

RESOURCES += \
    testresources.qrc
