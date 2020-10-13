#ifndef DEF_H
#define DEF_H

#include <QtCore>

#define WINDOW_ICO_SMALL       "/resources/ico/window_16x16.png"

#define PROGRAM_FILES_32        (QString::fromUtf8(qgetenv("ProgramFiles")) + "/")
#define PROGRAM_FILES_6432      (QString::fromUtf8(qgetenv("ProgramW6432")) + "/")

#define DEPENDENCY_DIR         (execPath + "/dependency/")
#define MAIN_WINDOW_TITLE      ("LYRA %s (non official wallet @ morgoth@devboard.tech)")

#ifdef Q_OS_UNIX
#define DOT_NET_CORE_NAME      ("packages-microsoft-prod.deb")
#define DOT_NET_CORE_WEB_SRC   ("https://packages.microsoft.com/config/ubuntu/18.04/packages-microsoft-prod.deb")
#elif defined(Q_OS_WIN32)
#define DOT_NET_CORE_NAME      ("netSdk3-1-402.exe")
#define DOT_NET_CORE_WEB_SRC   ("https://download.visualstudio.microsoft.com/download/pr/9706378b-f244-48a6-8cec-68a19a8b1678/1f90fd18eb892cbb0bf75d9cff377ccb/dotnet-sdk-3.1.402-win-x64.exe")
#endif

#define LYRA_CLI_VERSION       ("1.7.8.0")
#define LYRA_CLI_DLD_ADDR      ("https://github.com/LYRA-Block-Lattice/Lyra-Core/releases/download/" + QString(LYRA_CLI_VERSION) + "/lyra.permissionless-" + QString(LYRA_CLI_VERSION) + ".tar.bz2")
#define LYRA_CLI_DST_PATH      (execPath + "/")

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
