#-------------------------------------------------
#
# Project created by QtCreator 2010-09-15T21:01:59
#
#-------------------------------------------------

QT       += core gui

TARGET = SerialTest
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    ..\sidumprecord.cpp \
    ..\csidumper.cpp \
    ..\Utils.cpp \
    ..\qextserial\qextserialport.cpp

HEADERS  += mainwindow.h \
    utils.h \
    ..\qextserial\qextserialport_global.h \
    ..\qextserial\qextserialport.h \
    ..\qextserial\qextserialenumerator.h \
    ..\csidumper.h

INCLUDEPATH += ..
INCLUDEPATH += ..\qextserial

unix:SOURCES           += ..\qextserial\posix_qextserialport.cpp
unix:!macx:SOURCES     += ..\qextserial\qextserialenumerator_unix.cpp
macx {
  SOURCES          += ..\qextserial\qextserialenumerator_osx.cpp
  LIBS             += -framework IOKit -framework CoreFoundation
}

win32 {
  SOURCES          += ..\qextserial\win_qextserialport.cpp ..\qextserial\qextserialenumerator_win.cpp
  DEFINES          += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
  LIBS             += -lsetupapi
}

FORMS    += mainwindow.ui


# unix:LIBS += -L/usr/local/lib -lmath
LIBS += libsetupapi

