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
    importsidialog.cpp \
    CControlAdjustments.cpp \
    clockerrorsdialog.cpp \
    sidumprecord.cpp \
    csidumper.cpp \
    qextserial\qextserialport.cpp

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
    qextserial\qextserialport_global.h \
    qextserial\qextserialport.h \
    qextserial\qextserialenumerator.h \
    csidumper.h

INCLUDEPATH += qextserial

unix:SOURCES           += qextserial\posix_qextserialport.cpp
unix:!macx:SOURCES     += qextserial\qextserialenumerator_unix.cpp
macx {
  SOURCES          += qextserial\qextserialenumerator_osx.cpp
  LIBS             += -framework IOKit -framework CoreFoundation
}

win32 {
  SOURCES          += qextserial\win_qextserialport.cpp qextserial\qextserialenumerator_win.cpp
  DEFINES          += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
  LIBS             += -lsetupapi
}


FORMS    += mainwindow.ui \
    alterdialog.ui \
    coursesdialog.ui \
    coursedialog.ui \
    importsidialog.ui \
    clockerrorsdialog.ui

RESOURCES += \
    sisimple.qrc

win32:RC_FILE += win.rc
