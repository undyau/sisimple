#-------------------------------------------------
#
# Project created by QtCreator 2010-08-22T09:44:08
#
#-------------------------------------------------

QT       += core gui xml network svg widgets serialport

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
    importsidialog.cpp \
    CControlAdjustments.cpp \
    clockerrorsdialog.cpp \
    sidumprecord.cpp \
    csidumper.cpp \
    downloaddialog.cpp \
    ciofcoursexmlhandler.cpp \
    htmloptionsdialog.cpp \
    chtmloptions.cpp \
    cremediator.cpp

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
    importsidialog.h \
    CControlAdjustments.h \
    clockerrorsdialog.h \
    csidumper.h \
    downloaddialog.h \
    sidumprecord.h \
    ciofcoursexmlhandler.h \
    htmloptionsdialog.h \
    chtmloptions.h \
    version.h \
    cremediator.h

macx {
  LIBS             += -framework IOKit -framework CoreFoundation
}

win32 {
  DEFINES          += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
  LIBS             += -lsetupapi
}


FORMS    += mainwindow.ui \
    alterdialog.ui \
    coursesdialog.ui \
    coursedialog.ui \
    importsidialog.ui \
    clockerrorsdialog.ui \
    downloaddialog.ui \
    htmloptionsdialog.ui

RESOURCES += \
    sisimple.qrc

win32:RC_FILE += win.rc
