QT       += core gui
QT       += network
#unix:{
    # suppress the default RPATH if you wish
#QMAKE_LFLAGS_RPATH="\'\$$ORIGIN\'"
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
   #QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
#}

#DYLD_LIBRARY_PATH += "-Wl,-rpath,\'\$$ORIGIN\'"

#INCLUDEPATH += "-Wl,-rpath,\'\$$ORIGIN\'"

TARGET = lyra-gui-wallet-macOS-1.7.8.0-u3
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


QMAKE_CFLAGS_RELEASE = -Os -momit-leaf-frame-pointer

CONFIG += c++11

CONFIG-=app_bundle
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13

# RESOURCES     = lyra-gui-wallet.qrc

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
    send.cpp \
    pkeywindow.cpp

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
    send.h \
    pkeywindow.h

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
    pkeywindow.ui



TRANSLATIONS += \
    lyra-gui-wallet.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    lyra-gui-wallet.ts \
    resources/ico/copy.png \
    resources/ico/window_128x128.png \
    resources/ico/window_16x16.png \
    resources/ico/window_256x256.png \
    resources/ico/window_32x32.png \
    resources/ico/window_64x64.png