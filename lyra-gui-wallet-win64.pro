QT       += core gui
QT       += network

ICON = myapp.icns
RC_ICONS = your_icon.ico
TARGET = lyra-gui-wallet-win64-1.7.8.0-u2
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    create.cpp \
    debug.cpp \
    history.cpp \
    httpdownload.cpp \
    load.cpp \
    main.cpp \
    mainwindow.cpp \
    password.cpp \
    receive.cpp \
    send.cpp \
    showprivatekey.cpp

HEADERS += \
    about.h \
    create.h \
    debug.h \
    def.h \
    history.h \
    httpdownload.h \
    load.h \
    mainwindow.h \
    password.h \
    receive.h \
    send.h \
    showprivatekey.h

FORMS += \
    about.ui \
    create.ui \
    debug.ui \
    history.ui \
    load.ui \
    mainwindow.ui \
    password.ui \
    receive.ui \
    send.ui \
    showprivatekey.ui

TRANSLATIONS += \
    lyra-gui-wallet.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    lyra-gui-wallet.ts \
    resources/ico/window_128x128.png \
    resources/ico/window_16x16.png \
    resources/ico/window_256x256.png \
    resources/ico/window_32x32.png \
    resources/ico/window_64x64.png
