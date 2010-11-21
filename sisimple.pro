#-------------------------------------------------
#
# Project created by QtCreator 2010-08-22T09:44:08
#
#-------------------------------------------------

QT       += core gui

TARGET = sisimple
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Utils.cpp \
    CPunch.cpp \
    CLeg.cpp \
    CCourse.cpp \
    CSidetails.cpp \
    CLegstat.cpp \
    CXmlWriter.cpp \
    CResult.cpp \
    CUniquePunch.cpp \
    CEvent.cpp \
    alterdialog.cpp

HEADERS  += mainwindow.h \
    Utils.h \
    CPunch.h \
    CLeg.h \
    CCourse.h \
    CSidetails.h \
    CLegstat.h \
    CXmlWriter.h \
    CResult.h \
    CUniquePunch.h \
    CEvent.h \
    alterdialog.h

FORMS    += mainwindow.ui \
    alterdialog.ui

RESOURCES += \
    sisimple.qrc

win32:RC_FILE += win.rc
