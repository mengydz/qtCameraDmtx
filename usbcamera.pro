#-------------------------------------------------
#
# Project created by QtCreator 2016-05-23T23:13:27
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = Test
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        MainWindow.cpp \
        ImageWidget.cpp \
        libdmtx0.7.5/dmtx.c \
        libdmtx0.7.5/dmtxbytelist.c \
        libdmtx0.7.5/dmtxdecode.c \
        libdmtx0.7.5/dmtxdecodescheme.c \
        libdmtx0.7.5/dmtxencode.c \
        libdmtx0.7.5/dmtxencodeascii.c \
        libdmtx0.7.5/dmtxencodebase256.c \
        libdmtx0.7.5/dmtxencodec40textx12.c \
        libdmtx0.7.5/dmtxencodeedifact.c \
        libdmtx0.7.5/dmtxencodeoptimize.c \
        libdmtx0.7.5/dmtxencodescheme.c \
        libdmtx0.7.5/dmtxencodestream.c \
        libdmtx0.7.5/dmtximage.c \
        libdmtx0.7.5/dmtxmatrix3.c \
        libdmtx0.7.5/dmtxmessage.c \
        libdmtx0.7.5/dmtxplacemod.c \
        libdmtx0.7.5/dmtxreedsol.c \
        libdmtx0.7.5/dmtxregion.c \
        libdmtx0.7.5/dmtxscangrid.c \
        libdmtx0.7.5/dmtxsymbol.c \
        libdmtx0.7.5/dmtxtime.c \
        libdmtx0.7.5/dmtxvector2.c


HEADERS  += MainWindow.h \
            ImageWidget.h \
            libdmtx0.7.5/dmtx.h \
            libdmtx0.7.5/dmtxstatic.h

INCLUDEPATH += libdmtx0.7.5


FORMS    += MainWindow.ui

QMAKE_CXXFLAGS += -std=c++0x

win32:INCLUDEPATH += C:\opencv\build\include
win32:LIBS += -LC:\opencv\build\x86\vc12\lib

unix:LIBS += -lopencv_core -lopencv_highgui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
