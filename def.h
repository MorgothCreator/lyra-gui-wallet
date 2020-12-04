#ifndef DEF_H
#define DEF_H

#include <QtCore>

#define WINDOW_ICO_SMALL       ":/resources/ico/window_16x16.png"
#define COPY_ICO_SMALL         ":/resources/ico/copy.png"
#define ATTENTION_ICO_SMALL    ":/resources/ico/attention-small.png"
#define NEW_ICO_SMALL          ":/resources/ico/new-acc-small.png"
#define SEND_ICO_SMALL         ":/resources/ico/send_small.png"
#define RECEIVE_ICO_SMALL      ":/resources/ico/receive_small.png"

#define COPY_FADE_RATIO        (10)

#define PROGRAM_FILES_32       (QString::fromUtf8(qgetenv("ProgramFiles")) + "/")
#define PROGRAM_FILES_6432     (QString::fromUtf8(qgetenv("ProgramW6432")) + "/")

#define DEPENDENCY_DIR         (execPath + "/dependency/")
#define MAIN_WINDOW_TITLE      ("LYRA %s (non official wallet @ morgoth@devboard.tech)")

#if defined(Q_OS_OSX)
#define DOT_NET_CORE_NAME      ("dotnet-sdk-3.1.403-osx-x64.tar.gz")
#define DOT_NET_CORE_WEB_SRC   ("https://download.visualstudio.microsoft.com/download/pr/5e3b8ffc-c0c3-4d42-b207-87b91b678cf1/3994f505b0b91ed3858718c6dcd8cf9b/dotnet-sdk-3.1.403-osx-x64.tar.gz")
#elif defined (Q_OS_UNIX)
//#define DOT_NET_CORE_NAME      ("packages-microsoft-prod.deb")
//#define DOT_NET_CORE_WEB_SRC   ("https://packages.microsoft.com/config/ubuntu/18.04/packages-microsoft-prod.deb")
#define DOT_NET_CORE_NAME      ("dotnet-sdk-3.1.403-linux-x64.tar.gz")
#define DOT_NET_CORE_WEB_SRC   ("https://download.visualstudio.microsoft.com/download/pr/fdd9ecec-56b4-40f4-b762-d7efe24fc3cd/ffef51844c92afa6714528e10609a30f/dotnet-sdk-3.1.403-linux-x64.tar.gz")
#elif defined(Q_OS_WIN32)
//#define DOT_NET_CORE_NAME      ("netSdk3-1-402.exe")
//#define DOT_NET_CORE_WEB_SRC   ("https://download.visualstudio.microsoft.com/download/pr/9706378b-f244-48a6-8cec-68a19a8b1678/1f90fd18eb892cbb0bf75d9cff377ccb/dotnet-sdk-3.1.402-win-x64.exe")
#define DOT_NET_CORE_NAME      ("dotnet-sdk-3.1.403-win-x64.zip")
#define DOT_NET_CORE_WEB_SRC   ("https://download.visualstudio.microsoft.com/download/pr/143c00dc-a1f2-4070-a1fd-5f732e13be1f/b11931ba31bb5af69dce36e39a3014c5/dotnet-sdk-3.1.403-win-x64.zip")
#endif

//#if defined(Q_OS_OSX) || defined(Q_OS_WIN32)
#define DOTNET_ROOT_PATH       QString(DEPENDENCY_DIR + "dotnet/")
//#else
//#define DOTNET_ROOT_PATH       QString("")
//#endif

#define LYRA_PACKAGE_VERSION   ("1.7.11.6")
#define LYRA_CLI_VERSION       ("1.5.0")
#define LYRA_CLI_DLD_ADDR      ("https://github.com/LYRA-Block-Lattice/Lyra-Core/releases/download/" + QString(LYRA_PACKAGE_VERSION) + "/lyra.permissionless-" + QString(LYRA_PACKAGE_VERSION) + ".tar.bz2")
#ifdef Q_OS_OSX
#define LYRA_CLI_DST_PATH      (execPath)
#else
#define LYRA_CLI_DST_PATH      (execPath + "/")
#endif

#ifdef Q_OS_UNIX
#define ARCHIVER_NAME           "7z"
#elif defined(Q_OS_WIN32)
#define ARCHIVER_NAME           "7za.exe"
#endif

#ifdef Q_OS_UNIX
#define endLine        endl
#elif defined(Q_OS_WIN32)
#define endLine        Qt::endl
#endif

#endif // DEF_H
