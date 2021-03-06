QT       += core gui
QT       += network
unix:{
    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH=
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

ICON = myapp.icns
RC_ICONS = your_icon.ico
TARGET = lyra-gui-wallet
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

RESOURCES = lyra-gui-wallet.qrc

QMAKE_LFLAGS = -static-libgcc

QMAKE_CFLAGS_RELEASE = -Os -momit-leaf-frame-pointer
QMAKE_LFLAGS = -static-libgcc
DEFINES= QT_STATIC_BUILD
LFLAGS = -static -static-libgcc
#CONFIG = staticlib*


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutwindow.cpp \
    copyed.cpp \
    historywindow.cpp \
    namepasswindow.cpp \
    debugwindow.cpp \
    httpdownload.cpp \
    namewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    password.cpp \
    idwindow.cpp \
    responseparse.cpp \
    send.cpp \
    pkeywindow.cpp \
    qrcodegen.c \
    showqr.cpp

HEADERS += \
    aboutwindow.h \
    copyed.h \
    historywindow.h \
    namepasswindow.h \
    debugwindow.h \
    def.h \
    httpdownload.h \
    namewindow.h \
    mainwindow.h \
    password.h \
    idwindow.h \
    responseparse.h \
    send.h \
    pkeywindow.h \
    qrcodegen.h \
    showqr.h

FORMS += \
    aboutwindow.ui \
    copyed.ui \
    historywindow.ui \
    namepasswindow.ui \
    debugwindow.ui \
    namewindow.ui \
    mainwindow.ui \
    password.ui \
    idwindow.ui \
    send.ui \
    pkeywindow.ui \
    showqr.ui

TRANSLATIONS += \
    lyra-gui-wallet.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    lyra-gui-wallet.ts \
    resources/ico/attention-small.png \
    resources/ico/new-acc-small.png \
    resources/ico/receive_small.png \
    resources/ico/send_small.png \
    resources/ico/window_128x128.png \
    resources/ico/window_16x16.png \
    resources/ico/window_256x256.png \
    resources/ico/window_32x32.png \
    resources/ico/window_64x64.png
