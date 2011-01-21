#-------------------------------------------------
#
# Project created by QtCreator 2010-08-22T09:44:08
#
#-------------------------------------------------

QT       += core gui xml network svg

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
    alterdialog.cpp \
    coursesdialog.cpp \
    coursedialog.cpp \
    ciofresultxmlhandler.cpp \
    importsidialog.cpp

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
    alterdialog.h \
    coursesdialog.h \
    coursedialog.h \
    ciofresultxmlhandler.h \
    importsidialog.h

FORMS    += mainwindow.ui \
    alterdialog.ui \
    coursesdialog.ui \
    coursedialog.ui \
    importsidialog.ui

RESOURCES += \
    sisimple.qrc

win32:RC_FILE += win.rc
