#ifndef DEF_H
#define DEF_H

#define WINDOW_ICO_SMALL       "/resources/ico/window_16x16.png"

#define PROGRAM_FILES_32        (QString::fromUtf8(qgetenv("ProgramFiles")) + "/")
#define PROGRAM_FILES_6432      (QString::fromUtf8(qgetenv("ProgramW6432")) + "/")

#define DEPENDENCY_DIR         (execPath + "/dependency/")
#define MAIN_WINDOW_TITLE      ("LYRA %s (non official wallet @ morgoth@devboard.tech)")
#define DOT_NET_CORE_NAME      ("netSdk3-1-402.exe")
#define LYRA_CLI_VERSION       ("1.7.8.0")
#define LYRA_CLI_DLD_ADDR      ("https://github.com/LYRA-Block-Lattice/Lyra-Core/releases/download/" + QString(LYRA_CLI_VERSION) + "/lyra.permissionless-" + QString(LYRA_CLI_VERSION) + ".tar.bz2")
#define LYRA_CLI_DST_PATH      (execPath + "/")

#endif // DEF_H
