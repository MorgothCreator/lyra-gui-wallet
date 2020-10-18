#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <historywindow.h>

#include "pkeywindow.h"
#include "idwindow.h"
#include "send.h"
#include "password.h"
#include "namepasswindow.h"
#include "namewindow.h"

#include "httpdownload.h"
#include "responseparse.h"

#include "def.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    cliBusySemaphore = true;
#ifdef Q_OS_OSX
    execPath = QDir::homePath() + "/Documents/lyra";
    QDir root;
    root.mkdir(execPath);
    root.mkdir(DEPENDENCY_DIR);
#else
    execPath = QCoreApplication::applicationDirPath();
#endif
    QIcon windowIco = QIcon(execPath + WINDOW_ICO_SMALL);
    this->setWindowIcon(windowIco);

    attentionIco = new QIcon(execPath + ATTENTION_ICO_SMALL);
    newIco = new QIcon(execPath + NEW_ICO_SMALL);
    sendIco = new QIcon(execPath + SEND_ICO_SMALL);
    receiveIco = new QIcon(execPath + RECEIVE_ICO_SMALL);


    fee = 0;
    myId = "";
    myVotedId = "";
    numberOfBlocks = 0;
    myBalance = 0.0;

    ui->networkWarningLabel->setText("Please Wait, Loading necessary components...");
    ui->networkWarningLabel->setStyleSheet("font-weight: bold; color: red; font: bold 14px");

    if(ui->actionMainNet->isChecked()) {
        network = "mainnet";
    } else if(ui->actionTestNet->isChecked()) {
        network = "testnet";
    } else if(ui->actionDevNet->isChecked()) {
        network = "devnet";
    }
    this->setWindowTitle(QString::asprintf(MAIN_WINDOW_TITLE, network.toStdString().data()));


    homePath = QDir::homePath();
    loadConfig();

    modelHistory = new QStandardItemModel(0, 2, this);
    ui->hystoryListView->setModel(modelHistory);
    ui->hystoryListView->setIconSize(QSize(70,70));

    timerLoadWalletStart.setInterval(250);
    connect(&timerLoadWalletStart, SIGNAL(timeout()), this, SLOT(loadAtStart()));
    timerLoadWalletStart.start();


}

MainWindow::~MainWindow()
{
    if(lyraWalletProcess.state() == QProcess::Running) {
        exitCli();
    }
    delete ui;
}

void MainWindow::loadAtStart() {
    timerLoadWalletStart.stop();

#ifdef Q_OS_WIN32
    if(!installCppRedistForSsl()) {
        QMessageBox::critical(this, tr("ERROR"),
            tr("C++ Redistributable 2017: SSL need C++ Redistributable 2017 package in order to run.\n\r\n\r This GUI will start for demonstration only."));
        setWinTitle();
        this->setEnabled(true);
        return;
    }
    if(!installSsl()) {
        QMessageBox::critical(this, tr("ERROR"),
            tr("SSL: Without SSL package we can not download any \n\rnecessary package using https connections..\n\r\n\r This GUI will start for demonstration only."));
        setWinTitle();
        this->setEnabled(true);
        return;
    }
#endif
    if(!checkNetSdk()) {
        QMessageBox::critical(this, tr("ERROR"),
            tr("LYRA CLI wallet can not run without\n\r .NET Core 3.1.402.\n\r\n\r This GUI will start for demonstration only."));
        setWinTitle();
        this->setEnabled(true);
        return;
    }
    /* Force install, only for debug purposes. */
    //installSdk();
    if(!checkLyraCli()) {
            QMessageBox::critical(this, tr("ERROR"),
                tr("LYRA CLI wallet can not run without\n\r Downloading it.\n\r\n\r This GUI will start for demonstration only."));
            setWinTitle();
            this->setEnabled(true);
            return;
        }
    /* Force install, only for debug purposes. */
    // installLyraCli();

    setWinTitle();
    cliBusySemaphore = false;
    on_actionLoad_Wallet_triggered();
}

void MainWindow::dbg(QStringList data) {
    if(data.count() != 0) {
        err = data[data.count() - 1];
    }
    dbgWindow.append(data);
    qDebug() << data << endLine;
}

void MainWindow::dbg(QString data) {
    err = data;
    dbgWindow.append(data);
    qDebug() << data << endLine;
}

void MainWindow::initProgress(QString title) {
    loadWalletProgress = new QProgressDialog(this);
    loadWalletProgress->setMaximum(0);
    loadWalletProgress->setValue(0);
    loadWalletProgress->setWindowTitle(tr(title.toUtf8()));
    //loadWalletProgress->setGeometry(loadWalletProgress->x(), loadWalletProgress->y(), 350, loadWalletProgress->height());
    loadWalletProgress->resize(350, loadWalletProgress->height());
    QIcon pllIcon = QIcon(execPath + WINDOW_ICO_SMALL);
    loadWalletProgress->setWindowIcon(pllIcon);
    loadWalletProgress->show();
}

void MainWindow::deleteProgress() {
    delete loadWalletProgress;
}


bool MainWindow::installSdk() {
    dbg("INFO 'Install .NET Core': NET SDK 3.1 not found.");
    dbg("INFO 'Install .NET Core': We will download and install NET SDK 3.1.");



#if defined(Q_OS_OSX) || defined(Q_OS_WIN32) || defined (Q_OS_UNIX)
    httpDownload sdkDownload(this);
    if(!sdkDownload.getSizeEqual(new QString(DEPENDENCY_DIR + DOT_NET_CORE_NAME), new QString(DOT_NET_CORE_WEB_SRC))) {
        if(!sdkDownload.download(QString("Download " + QString(DOT_NET_CORE_NAME)), new QString(DEPENDENCY_DIR + DOT_NET_CORE_NAME), new QString(DOT_NET_CORE_WEB_SRC))) {
            return false;
        }
    }
    QDir root;
    root.mkdir(DEPENDENCY_DIR + "dotnet/");
    initProgress("Decompressing .NET Core in progress...");

    QString program = "tar";
    QStringList arguments;
    arguments << "-xzvf";
    arguments << DEPENDENCY_DIR + DOT_NET_CORE_NAME;
    arguments << "-C";
    arguments << DEPENDENCY_DIR + "dotnet/";

    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);
    //QStringList rec;
    if(lyraWalletProcess.waitForStarted(5000)) {
        //lyraWalletProcess.waitForFinished();
        while(lyraWalletProcess.state() == QProcess::Running) {
            QApplication::processEvents();
        }
        lyraWalletProcess.close();
    } else {
        dbg("ERROR 'Decompress LYRA CLI': tar not starting.");
        deleteProgress();
        return false;
    }
#elif defined (Q_OS_UNIX)
    initProgress("Decompressing .NET Core in progress...");
    dbg("ERROR 'Install .NET Core': Starting instalation.");
    QString program = "gnome-terminal";
    QStringList arguments;
    arguments << "--wait";
    arguments << "-e";
    arguments << "bash -c './dependency/ubuntuSdkCoreInstall.sh'";
    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.execute(program, arguments);
    if(lyraWalletProcess.waitForStarted(5000)) {
        //lyraWalletProcess.waitForFinished();
        //qint64 procId = lyraWalletProcess.processId();
        //dbg(QString::number(procId));
        while(lyraWalletProcess.state() == QProcess::Running) {
            QApplication::processEvents();
        }
        lyraWalletProcess.close();
        dbg("ERROR 'Install .NET Core': Install script.");
        deleteProgress();
        return false;
    }
    dbg("ERROR 'Install .NET Core': Instalation ended.");
#elif defined(Q_OS_WIN32)
/*    httpDownload sdkDownload(this);
    if(!sdkDownload.getSizeEqual(new QString(DEPENDENCY_DIR + DOT_NET_CORE_NAME), new QString(DOT_NET_CORE_WEB_SRC))) {
        if(!sdkDownload.download(QString("Download " + QString(DOT_NET_CORE_NAME)), new QString(DEPENDENCY_DIR + DOT_NET_CORE_NAME), new QString(DOT_NET_CORE_WEB_SRC))) {
            QMessageBox::critical(this, tr("ERROR"),
                                     tr("LYRA CLI wallet can not run without\n\r .NET Core 3.1.\n\r\n\r This GUI will start for demonstration only."));
            return false;
        }
    }

    initProgress("Decompressing .NET Core in progress...");
    dbg("INFO 'Install .NET Core': Starting instalation.");
    QString program = DEPENDENCY_DIR + DOT_NET_CORE_NAME;
    QStringList arguments;
    arguments << "/passive";
    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);
    if(lyraWalletProcess.waitForStarted(5000)) {
        //lyraWalletProcess.waitForFinished();
        // Commenting, it freezes here.
        //while(lyraWalletProcess.state() == QProcess::Running) {
            QApplication::processEvents();
        }
        lyraWalletProcess.close();
        dbg("INFO 'Install .NET Core': Instalation ended.");
    } else {
        dbg("ERROR 'Install .NET Core': Installer not starting.");
        deleteProgress();
        return false;
    }*/
#endif
    deleteProgress();
    return true;
}

bool MainWindow::checkNetSdk() {
    QString program = DOTNET_ROOT_PATH + "dotnet";
    QStringList arguments;
    arguments << "--version";
    QProcess netSdk;
    netSdk.setParent(this);
    netSdk.setReadChannel(QProcess::StandardOutput);
    netSdk.start(program, arguments);
    if(netSdk.waitForStarted(2000)) {
#ifdef Q_OS_OSX
        while(lyraWalletProcess.state() == QProcess::Running);
#else
        netSdk.waitForFinished();
#endif
        netSdk.waitForReadyRead();
        QString sdkCliRead = netSdk.readAll();
        netSdk.close();
        QStringList sdkVersions = sdkCliRead.split("\n");
        sdkVersions.removeAll("");
        for(QString line : sdkVersions) {
            dbg("INFO 'Check .NET Core': NET SDK Version: " + line.remove("\r"));
        }
        if(sdkCliRead.contains("3.1")) {
            dbg("INFO 'Check .NET Core': NET SDK 3.1 found.");
        } else {
            return installSdk();
        }
    } else {
        return installSdk();
    }
    return true;
}

bool MainWindow::installLyraCli() {
    //if(getSizeEqual(this,))
    httpDownload sdkDownload(this);
    if(!sdkDownload.getSizeEqual(new QString(DEPENDENCY_DIR + QString(LYRA_CLI_VERSION) + ".tar.bz2"), new QString(LYRA_CLI_DLD_ADDR))) {
        dbg("INFO 'Download LYRA CLI': LYRA CLI wallet archive not found.");
        dbg("INFO 'Download LYRA CLI': Proceed to download LYRA CLI wallet.");
        if(!sdkDownload.download(QString("Download lyra.permissionless-" + QString(LYRA_CLI_VERSION) + ".tar.bz2"), new QString(DEPENDENCY_DIR + QString(LYRA_CLI_VERSION) + ".tar.bz2"), new QString(LYRA_CLI_DLD_ADDR))) {
            dbg("ERROR 'Download LYRA CLI': Downloading LYRA CLI archive.");
            return false;
        } else {
            dbg("INFO 'Download LYRA CLI': Download LYRA CLI wallet succesful.");
        }
    }

#if defined(Q_OS_UNIX) || defined(Q_OS_OSX)
    initProgress("Decompressing LYRA CLI in progress...");
    QString program = "tar";
    QStringList arguments;
    arguments << "-xvjf";
    arguments << DEPENDENCY_DIR + LYRA_CLI_VERSION + ".tar.bz2";
    arguments << "-C";
    arguments << LYRA_CLI_DST_PATH;
    //lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);
    //QStringList rec;
    if(lyraWalletProcess.waitForStarted(5000)) {
        //lyraWalletProcess.waitForFinished();
        while(lyraWalletProcess.state() == QProcess::Running) {
            QApplication::processEvents();
        }
        lyraWalletProcess.close();
    } else {
        dbg("ERROR 'Decompress LYRA CLI': tar not starting.");
        deleteProgress();
        return false;
    }
#elif defined(Q_OS_WIN32)
    /* tar on windows does not know to handle tar.bz2 archives, so we use 7z instead. */

    initProgress("Decompressing LYRA CLI in progress...");
    QString program = DEPENDENCY_DIR + ARCHIVER_NAME;
    QStringList arguments;
    arguments << "x";
    arguments << DEPENDENCY_DIR + LYRA_CLI_VERSION + ".tar.bz2";
    arguments << "-aoa";
    arguments << "-o" + DEPENDENCY_DIR;
    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);
    //QStringList rec;
    if(lyraWalletProcess.waitForStarted(5000)) {
        lyraWalletProcess.waitForFinished();
        while(lyraWalletProcess.state() == QProcess::Running);
        lyraWalletProcess.close();
        arguments.clear();
        arguments << "x";
        arguments << DEPENDENCY_DIR + LYRA_CLI_VERSION + ".tar";
        arguments << "-aoa";
        arguments << "-o" + LYRA_CLI_DST_PATH;
        lyraWalletProcess.start(program, arguments);
        if(lyraWalletProcess.waitForStarted(5000)) {
            //lyraWalletProcess.waitForFinished();
            while(lyraWalletProcess.state() == QProcess::Running) {
                QApplication::processEvents();
            }
            lyraWalletProcess.close();
            QFile fil(DEPENDENCY_DIR + LYRA_CLI_VERSION + ".tar");
            fil.remove();
            dbg("INFO 'Decompress LYRA CLI': LYRA CLI wallet extracted.");
        } else {
            dbg("ERROR 'Decompress LYRA CLI': 7z not starting.");
            deleteProgress();
            return false;
        }
    } else {
        dbg("ERROR 'Decompress LYRA CLI': 7z not starting.");
        deleteProgress();
        return false;
    }
#endif
    deleteProgress();
    return true;
}

bool MainWindow::checkLyraCli() {
    if(!QFile::exists(DOTNET_ROOT_PATH + "dotnet")) {
        dbg("ERROT 'Check LYRA CLI': .NET SDK does not exist at the desired location.");
        checkNetSdk();
    } else {
            if(!QFile::exists(execPath + "/lyra/cli/lyra.dll")) {
            dbg("INFO 'Check LYRA CLI': LYRA CLI not found at the desired location.");
            return installLyraCli();
        }
    }
    if(!QFile::exists(execPath + "/lyra/cli/lyra.dll")) {
        dbg("INFO 'Check LYRA CLI': LYRA CLI does not exist at the desired location.");
        return installLyraCli();
    }

    QString program = DOTNET_ROOT_PATH + "dotnet";
    QStringList arguments;
    arguments << execPath + "/lyra/cli/lyra.dll" << "--version";
    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    lyraWalletProcess.start(program, arguments);
    if(lyraWalletProcess.waitForStarted(2000)) {
        lyraWalletProcess.waitForFinished();
        QString sdkCliRead = lyraWalletProcess.readAll();
        lyraWalletProcess.close();
        exitCli();
        QStringList sdkVersions = sdkCliRead.split("\n");
        sdkVersions.removeAll("");
        for(QString line : sdkVersions) {
            dbg("INFO 'Check LYRA CLI': LYRA CLI version: " + line.remove("\r"));
        }
        if(sdkCliRead.contains("LYRA Block Lattice Command Line Client")) {
            dbg("INFO 'Check LYRA CLI': LYRA CLI version: " + QString(LYRA_CLI_VERSION) + " found.");
        } else {
            return installLyraCli();
        }
    } else {
        return installLyraCli();
    }
    return true;
}

#ifdef Q_OS_WIN32
bool MainWindow::installSsl () {
    /*
     * Look for Open SSL executable in program files, if is not found, install it.
     */
    if(!QFile::exists(PROGRAM_FILES_6432 + "OpenSSL-Win64/bin/openssl.exe")) {
        dbg("INFO 'Install SSL': Open SSL not installed on this machine.");
        dbg("INFO 'Install SSL': Proced to instal Open SSL...");
        QString program = DEPENDENCY_DIR + "Win64OpenSSL_Light-1_1_1h";
        QStringList arguments;
        arguments << "/silent";
        lyraWalletProcess.setParent(this);
        lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
        connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
        lyraWalletProcess.start(program, arguments);
        if(lyraWalletProcess.waitForStarted(5000)) {
            dbg("INFO 'Install SSL': Open SSL installer started.");
            lyraWalletProcess.waitForFinished();
            while(lyraWalletProcess.state() == QProcess::Running);
            lyraWalletProcess.close();
        } else {
            dbg("ERROR 'Install SSL': SSL installer not starting.");
            return false;
        }
    } else {
        dbg("INFO 'Install SSL': Open SSL already installed on this PC.");
        return true;
    }
    return true;
}
#endif

#ifdef Q_OS_WIN32
bool MainWindow::installCppRedistForSsl () {
    /*
     * Look for Open SSL executable in program files, if is not found, install it.
     */
    dbg("INFO '': Search for C++ 2017 Redistributable on this machine.");
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", QSettings::NativeFormat);
    QString cppRedVersion = reg.value("Version").toString();
    if(cppRedVersion.length()) {
        dbg("INFO 'Install C++ Redist': C++ 2017 Redistributable " + cppRedVersion + " found.");
    } else {
        dbg("INFO 'Install C++ Redist': C++ 2017 Redistributable " + cppRedVersion + " not found.");
    }
    qlonglong cppRedVersionValue = cppRedVersion.replace(".", "").replace("v", "").toLongLong();
    if(cppRedVersionValue >= qlonglong(14162703300)) {
        dbg("INFO 'Install C++ Redist': C++ 2017 Redistributable " + cppRedVersion +" found on PC and is newer than the one in this package.");
        return true;
    }

    if(QFile::exists(DEPENDENCY_DIR + "VC_redist.x64.exe")) {
        dbg("INFO 'Install C++ Redist': Local C++ 2017 Redistributable installer found");
        QString program = DEPENDENCY_DIR + "VC_redist.x64.exe";
        QStringList arguments;
        arguments << "/passive";
        lyraWalletProcess.setParent(this);
        lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
        connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
        lyraWalletProcess.start(program, arguments);
        if(lyraWalletProcess.waitForStarted(5000)) {
            dbg("INFO 'Install C++ Redist': C++ 2017 Redistributable installer started.");
            lyraWalletProcess.waitForFinished();
            while(lyraWalletProcess.state() == QProcess::Running);
            lyraWalletProcess.close();
        } else {
            dbg("ERROR 'Install C++ Redist': SSL installer not starting.");
            return false;
        }
    } else {
        dbg("ERROR 'Install C++ Redist': VC_redist.x64.exe installer not found.");
        return false;
    }
    return true;
}
#endif

void MainWindow::setWinTitle() {
    ui->networkWarningLabel->setText("You are on " + network + " network");
    if(!network.compare("mainnet")) {
        ui->networkWarningLabel->setStyleSheet("font-weight: bold; color: red; font: bold 22px");
    } else if(!network.compare("testnet")) {
        ui->networkWarningLabel->setStyleSheet("font-weight: bold; color: green; font: bold 22px");
    } else {
        ui->networkWarningLabel->setStyleSheet("font-weight: bold; color: blue; font: bold 22px");
    }
}

bool MainWindow::createWallet(QString name, QString pass) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        dbg("INFO 'New wallet': LYRA CLI found open, now we wil close it to open another instance.");
        exitCli();
    }
    QString program = DOTNET_ROOT_PATH + "dotnet";
    QStringList arguments;
    arguments << execPath + "/lyra/cli/lyra.dll" << "--networkid" << network << "-p" << "webapi";

    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);

    loadWalletProgress->setMaximum(5);
    loadWalletProgress->setValue(0);
    loadWalletProgress->setWindowTitle(tr("Creating wallet..."));

    if(lyraWalletProcess.waitForStarted(5000)) {
        dbg("INFO 'New wallet': LYRA CLI wallet instance started.");
        loadWalletProgress->setValue(1);
        if(expectResponse(nullptr, QString("Press Enter for default account"), nullptr, 5000)) {
            loadWalletProgress->setValue(2);
            dbg("INFO 'New wallet': LYRA CLI wallet responded as expected.");
            QString set = name + "\n";
            if(expectResponse(&set, QString("Local account data not found."), nullptr, 5000)) {
                loadWalletProgress->setValue(3);
                set = "Y\n";
                if(expectResponse(&set, QString("Please input a password:"), nullptr, 5000)) {
                    loadWalletProgress->setValue(4);
                    set = pass + "\n";
                    QStringList rec;
                    if(expectResponse(&set, walletName, &rec, 5000)) {
                        dbg(QString("INFO 'New wallet': ") + "'" + name + "' LYRA wallet successfully opened.");
                        loadWalletProgress->setValue(5);
                        for(QString line : rec) {
                            QStringList sp = line.split(": ");
                            if(sp[0].contains("Transfer Fee")) {
                                QString f = sp[1].replace(',', '.');
                                fee = f.toDouble();
                            } else if(sp[0].contains("Current voting for Account Id")) {
                                myVotedId = sp[1];
                            } else if(sp[0].contains("Account Id")) {
                                myId = sp[1];
                            } else if(sp[0].contains("Number of Blocks")) {
                                numberOfBlocks = sp[1].toInt();
                            } else if(sp[0].contains("LYR")) {
                                QString bal = sp[0].left(sp[0].length() - 4).replace(',', '.');
                                bool ok = false;
                                myBalance = bal.toDouble(&ok);
                            } else if(sp[0].contains("Balance")) {
                                myBalance = sp[1].toDouble();
                            }
                        }
                        ui->balanceLabel->setText(QString::asprintf("%.8f", myBalance) + " LYR");
                        readHistory(false);
                    } else {
                        dbg("ERROR 'New wallet': Unknown.");
                        return false;
                    }
                } else {
                    dbg("ERROR 'New wallet': Invalid password format/ wrong password.");
                    return false;
                }
            } else {
                dbg("ERROR 'New wallet': Wallet already exist.");
                return false;
            }
        } else {
            dbg("ERROR 'New wallet': Unrecognized response from cli.");
            return false;
        }
    } else {
        dbg("ERROR 'New wallet': Wallet cli not found at given location.");
        return false;
    }
    return true;
}

bool MainWindow::recoverWallet(QString name, QString key, QString pass) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        dbg("INFO 'New wallet': LYRA CLI found open, now we wil close it to open another instance.");
        exitCli();
    }
    QString program = DOTNET_ROOT_PATH + "dotnet";
    QStringList arguments;
    arguments << execPath + "/lyra/cli/lyra.dll" << "--networkid" << network << "-p" << "webapi";

    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);

    loadWalletProgress->setMaximum(6);
    loadWalletProgress->setValue(0);
    loadWalletProgress->setWindowTitle(tr("Recovering wallet..."));
    if(lyraWalletProcess.waitForStarted(5000)) {
        dbg("INFO 'Recover wallet': LYRA CLI wallet instance started.");
        loadWalletProgress->setValue(1);
        if(expectResponse(nullptr, QString("Press Enter for default account"), nullptr, 5000)) {
            loadWalletProgress->setValue(2);
            dbg("INFO 'Recover wallet': LYRA wallet responded as expected.");
            QString set = name + "\n";
            if(expectResponse(&set, QString("Local account data not found."), nullptr, 5000)) {
                loadWalletProgress->setValue(3);
                set = "n\n";
                if(expectResponse(&set, QString("Please enter private key to restore account"), nullptr, 5000)) {
                    loadWalletProgress->setValue(4);
                    set = key + "\n";
                    if(expectResponse(&set, QString("Please input a password"), nullptr, 5000)) {
                        loadWalletProgress->setValue(5);
                        set = pass + "\n";
                        QStringList rec;
                        if(expectResponse(&set, walletName, &rec, 10000)) {
                            dbg("INFO '': " + name + " LYRA wallet successfully opened.");
                            loadWalletProgress->setValue(6);
                            for(QString line : rec) {
                                QStringList sp = line.split(": ");
                                if(sp[0].contains("Transfer Fee")) {
                                    QString f = sp[1].replace(',', '.');
                                    fee = f.toDouble();
                                } else if(sp[0].contains("Current voting for Account Id")) {
                                    myVotedId = sp[1];
                                } else if(sp[0].contains("Account Id")) {
                                    myId = sp[1];
                                } else if(sp[0].contains("Number of Blocks")) {
                                    numberOfBlocks = sp[1].toInt();
                                }
                            }
                            readHistory(false);
                            syncAccount();
                        } else {
                            dbg("ERROR 'Recover wallet': Unknown.");
                            return false;
                        }
                    } else {
                        dbg("ERROR 'Recover wallet': Invalid password format/ wrong password.");
                        return false;
                    }
                } else {
                    dbg("ERROR 'Recover wallet': Invalid private key.");
                    return false;
                }
            } else {
                dbg("ERROR 'Recover wallet': Wallet already exist.");
                return false;
            }
        } else {
            dbg("ERROR 'Recover wallet': Unrecognized response from cli.");
            return false;
        }
    } else {
        dbg("ERROR 'Recover wallet': Wallet cli not found at given location.");
        return false;
    }
    return true;
}

bool MainWindow::openWallet(QString name, QString pass) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        dbg("INFO 'New wallet': LYRA CLI found open, now we wil close it to open another instance.");
        exitCli();
    }
    QString program = DOTNET_ROOT_PATH + "dotnet";
    QStringList arguments;
    arguments << execPath + "/lyra/cli/lyra.dll" << "--networkid" << network << "-p" << "webapi";

    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    //lyraWalletProcess.setProcessChannelMode((QProcess::MergedChannels));

    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);

    loadWalletProgress->setMaximum(5);
    loadWalletProgress->setValue(0);
    loadWalletProgress->setWindowTitle(tr("Opening wallet..."));
    if(lyraWalletProcess.waitForStarted(5000)) {
        dbg("INFO 'Open wallet': LYRA CLI wallet instance started.");
        loadWalletProgress->setValue(1);
        if(expectResponse(nullptr, QString("Press Enter for default account"), nullptr, 5000)) {
            loadWalletProgress->setValue(2);
            dbg("INFO 'Open wallet': LYRA wallet responded as expected.");
            QString set;
            if(name == nullptr) {
                getConfig(QString("wall-name"), set);
            } else {
                set = name + "\n";
            }
            if(expectResponse(&set, QString("Please input a password to open wallet"), nullptr, 5000)) {
                loadWalletProgress->setValue(3);
                set = pass + "\n";
                if(expectResponse(&set, QString("Type 'help' to see the list"), nullptr, 5000)) {
                    loadWalletProgress->setValue(4);
                    QStringList rec;
                    //set = "";
                    expectResponse(nullptr, walletName, &rec, 5000);
                    expectResponse(nullptr, walletName, &rec, 5000);
                    loadWalletProgress->setValue(5);
                    dbg("INFO 'Open wallet': " + name + " LYRA wallet successfully opened.");
                    for(QString line : rec) {
                        QStringList sp = line.split(": ");
                        if(sp[0].contains("Transfer Fee")) {
                            QString f = sp[1].replace(',', '.');
                            fee = f.toDouble();
                        } else if(sp[0].contains("Current voting for Account Id")) {
                            myVotedId = sp[1];
                        } else if(sp[0].contains("Account Id")) {
                            myId = sp[1];
                        } else if(sp[0].contains("Number of Blocks")) {
                            numberOfBlocks = sp[1].toInt();
                        } else if(sp[0].contains("LYR")) {
                            QString bal = sp[0].left(sp[0].length() - 4).replace(',', '.');
                            myBalance = bal.toDouble();
                        } else if(sp[0].contains("Balance")) {
                            myBalance = sp[1].toDouble();
                        }
                    }
                    ui->balanceLabel->setText(QString::asprintf("%.8f", myBalance) + " LYR");
                    readHistory(false);
                } else {
                    dbg("ERROR 'Open wallet': Invalid password/ wrong password.");
                    return false;
                }
            } else {
                dbg("ERROR 'Open wallet': Invalid wallet name.");
                return false;
            }

        } else {
            dbg("ERROR 'Open wallet': Opening wallet cli.");
            return false;
        }
    }
    else
    {
        dbg("ERROR 'Open wallet': Wallet cli not found at given location.");
        return false;
    }
    return true;
}

bool MainWindow::sendCoins(QString id, double value) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        QString set = "send\n";
        loadWalletProgress->setMaximum(2);
        loadWalletProgress->setValue(0);
        loadWalletProgress->setWindowTitle(tr("Sending LYR coins..."));
        if(expectResponse(&set, QString("Please enter destination account id"), nullptr, 5000)) {
            loadWalletProgress->setValue(1);
            set = id + "\n";
            dbg("INFO 'Send coins': ID Sending: " + set);
            if(expectResponse(&set, QString("Please enter amount"), nullptr, 5000)) {
                loadWalletProgress->setValue(2);
                QLocale sys = QLocale::system();
                set = QString::asprintf("%.8f", value) + "\n";
#ifdef Q_OS_WIN32
                if(sys.decimalPoint() == ',') {
                    set = set.replace(".", ",");
                }
#endif
                dbg("INFO 'Send coins': Coins sending:" + set);
                if(expectResponse(&set, QString("Send Transfer block has been authorized successfully"), nullptr, 10000)) {
                    dbg(QString("INFO '': Coins send: ") + QString::asprintf("%.8f", value) + QString("LYR, to id: ") + id);
                    syncAccount();
                } else {
                    dbg("ERROR 'Send coins': Invalid value.");
                    return false;
                }
            } else {
                dbg("ERROR 'Send coins': Invalid id.");
                return false;
            }
        } else {
            dbg("ERROR 'Send coins': Unnexpected response from cli.");
        }
    } else {
        dbg("ERROR 'Send coins': No wallet opened.");
        return false;
    }
    return true;
}

bool MainWindow::syncAccount() {
    if(lyraWalletProcess.state() == QProcess::Running) {
        QString set = "sync\n";
        loadWalletProgress->setMaximum(2);
        loadWalletProgress->setValue(0);
        loadWalletProgress->setWindowTitle(tr("Sincyng account..."));
        dbg("INFO 'Sync account': Sync in progress." + set);
        if(expectResponse(&set, QString("Sync Result: Success"), nullptr, 10000)) {
            loadWalletProgress->setValue(1);
            dbg("INFO 'Sync account': Sync successfull.");
            set = "balance\n";
            QStringList rec;
            if(expectResponse(&set, walletName, &rec, 5000)) {
                loadWalletProgress->setValue(2);
                dbg("INFO 'Sync account': Parse balance.");
                for(QString line : rec) {
                    if(line.contains("LYR")) {
                        QString bal = line.left(line.length() - 4).replace(',', '.');
                        myBalance = bal.toDouble();
                        ui->balanceLabel->setText(QString::asprintf("%.8f", myBalance) + " LYR");
                        break;
                    } else if(!line.compare("0")) {
                        myBalance = line.toDouble();
                        ui->balanceLabel->setText(QString::asprintf("%.8f", myBalance) + " LYR");
                        break;
                    }
                }
                readHistory(true);
            } else {
                dbg("ERROR 'Sync account': Unnexpected response from cli.");
                return false;
            }
        } else {
            dbg("ERROR 'Sync account': Unnexpected response from cli.");
            return false;
        }
    } else {
        dbg("ERROR 'Sync account': No wallet opened.");
        return false;
    }
    return true;
}

bool MainWindow::showPrivateKey(QString *key) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        QString set = "key\n";
        QStringList rec;
        expectResponse(&set, walletName, &rec, 1000);
        if(rec.count() == 2) {
            *key = rec[0];
        } else {
            dbg("ERROR 'Show private key': Invalid response.");
            return false;
        }
    } else {
        dbg("ERROR 'Show private key': No wallet opened.");
        return false;
    }
    return true;
}

bool MainWindow::showId(QString *id) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        QString set = "id\n";
        QStringList rec;
        expectResponse(&set, walletName, &rec, 1000);
        if(rec.count() == 2) {
            *id = rec[0];
        } else {
            dbg("ERROR 'Show ID': Invalid response.");
            return false;
        }
    } else {
        dbg("ERROR 'Show ID': No wallet opened.");
        return false;
    }
    return true;
}

bool MainWindow::voteFor(QString id) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        QString set = "votefor\n";
        QStringList rec;
        if(expectResponse(&set, "Please", &rec, 1000)) {
            set = id + "\n";
            if(expectResponse(&set, id, &rec, 5000) && rec.count() == 2) {
                QString tmp;
                for(QString line : rec) {
                    tmp.append(line.replace(". ", ".\n\r"));
                }
            } else {
                dbg("ERROR 'Wote for': Unknown error");
                return false;
            }
        } else {
            dbg("ERROR 'Wote for': Entering vote procedure.");
            return false;
        }
    } else {
        dbg("ERROR 'Wote for': No wallet opened.");
        return false;
    }
    return true;
}

bool MainWindow::readHistory(bool update) {
    QThread::sleep(1);
    if(lyraWalletProcess.state() == QProcess::Running) {
        QString set = "count\n";
        QStringList rec;
        expectResponse(&set, walletName, &rec, 500);
        if(rec.count() == 2) {
            bool ok = false;
            int count = rec[0].remove("\n").remove("\r").toUInt(&ok);
            if(ok) {
                if(count) {
                    if(!update) {
                        if(modelHistory) {
                            delete modelHistory;
                        }
                        lastAmount = 0;
                        history.clear();
                        modelHistory = new QStandardItemModel(0, 2, this);
                        ui->hystoryListView->setModel(modelHistory);
                    }
                    for(int cnt = modelHistory->rowCount() + 1; cnt <= count; cnt++) {
                        set = "print\n";
                        if(expectResponse(&set, QString("Please enter transaction block index"), nullptr, 1000)) {
                            set = QString::number(cnt) + "\n";
                            rec.clear();
                            expectResponse(&set, walletName, &rec, 5000);
                            QStandardItem *item = new QStandardItem();
                            if(item != 0) {
                                QString tmpItemTxt;
                                for(QString line : rec) {
                                    tmpItemTxt.append(line + "\n");
                                }
                                QStringList tmpList = responseParse::print(tmpItemTxt, {"BlockType:", "Height:", "Balances:", "Fee:"});
                                QString listBoxItemText = "";
                                tmpList[2] = tmpList[2].insert(tmpList[2].length() - 8, '.');
                                listBoxItemText.append("BlockType: " + tmpList[0] + "\n");
                                listBoxItemText.append("Transfer Nr: " + tmpList[1] + "\n");
                                listBoxItemText.append("Balance: " + tmpList[2] + "\n");
                                listBoxItemText.append("Fee: " + tmpList[3] + "\n");

                                QStringList currentAmount = tmpList[2].split(":");
                                double amountTransfered = 0;
                                if(currentAmount[0].contains("LYR")) {
                                    amountTransfered = currentAmount[1].toDouble() + tmpList[3].toDouble() - lastAmount;
                                } else {
                                    amountTransfered = 0;
                                }
                                lastAmount = currentAmount[1].toDouble();
                                if(amountTransfered < 0) {
                                    amountTransfered = 0 - amountTransfered;
                                }
                                QString amm = QString::asprintf("%.8f", amountTransfered) + "LYR";

                                if(listBoxItemText.contains("OpenAccountWithReceiveTransfer")) {
                                    item->setIcon(*newIco);
                                    QStringList ammCur = tmpList[2].split(":");
                                    listBoxItemText.append("Amount received: " + amm + "\n");
                                } else if(listBoxItemText.contains("SendTransfer")) {
                                    item->setIcon(*sendIco);
                                    listBoxItemText.append("Amount sent: " + amm + "\n");
                                } else if(listBoxItemText.contains("ReceiveTransfer")) {
                                    item->setIcon(*receiveIco);
                                    listBoxItemText.append("Amount received: " + amm + "\n");
                                } else {
                                    item->setIcon(*attentionIco);
                                }
                                item->setText(listBoxItemText);
                                item->setEditable(false);
                                ui->hystoryListView->setStyleSheet("font-weight: bold; color: black; font: bold 12px");
                                history.insert(0, tmpItemTxt);
                                modelHistory->insertRow(0, item);
                            }
                        }
                    }
                } else {
                    dbg("INFO 'Read history': No transaction found.");
                    return false;
                }
            } else {
                dbg("ERROR 'Read history': Invalid response.");
                return false;
            }
        } else {
            dbg("ERROR 'Read history': Invalid response.");
            return false;
        }
    }

    return true;
}

bool MainWindow::expectResponse(QString *command, QString expectedResponse, QStringList *receivedData, int timeout){
    lyraWallLineResponse.clear();
    if(command != nullptr && command->length() != 0) {
        lyraWalletProcess.write(command->toUtf8());
    }
    int cnt = timeout / 25;
    QStringList tmpData;
    while(1) {
        if(!cnt) {
            if(tmpData.count() != 0 && receivedData != nullptr) {
                receivedData->clear();
                for(QString line : tmpData) {
                    receivedData->append(line.remove('\n').remove('\r'));
                }
                receivedData->removeAll(QString(""));
                return true;
            } else if(receivedData == nullptr) {
                return false;
            }
        }
        QThread::msleep(25);
        QApplication::processEvents();
        cnt--;
        if(lyraWallLineResponse.count()) {
            cnt = timeout / 25;
        }
        tmpData.append(lyraWallLineResponse);
        dbgWindow.append(lyraWallLineResponse);
        lyraWallLineResponse.clear();
        for(QString line : tmpData) {
            if(receivedData != nullptr) {
                receivedData->append(line.remove('\n').remove('\r'));
                receivedData->removeAll(QString(""));
            }
            if(expectedResponse.length() != 0 && line.contains(expectedResponse)) {
                return true;
            }
        }
    };
}

void MainWindow::exitCli() {
    ui->balanceLabel->setText(QString::asprintf("%.8f", 0.0) + " LYR");
    lyraWalletProcess.write(QString("stop\n").toUtf8());

    int cnt = 0;
    while(lyraWalletProcess.state() == QProcess::Running) {
        QEventLoop loop;
        QTimer::singleShot(250, &loop, SLOT(quit()));
        loop.exec();
        if(cnt == 10) {
            lyraWalletProcess.kill();
            QEventLoop loop;
            QTimer::singleShot(2000, &loop, SLOT(quit()));
            loop.exec();
            dbg("ERROR 'Exit CLI': LYRA CLI killed.");
            return;
        }
        cnt++;
    }
    lyraWalletProcess.waitForFinished();
    while(lyraWalletProcess.state() == QProcess::Running);
    lyraWalletProcess.close();
    dbg(lyraWalletProcess.readAll());
}

bool MainWindow::getConfig(QString name, QString &response) {
    bool found = false;
    QStringList spl;
    for(QString line : config) {
        spl = line.split("=");
        if(!spl[0].compare(name)) {
            found = true;
            break;
        }
    }
    if(!found) {
        return false;
    }
    response = spl[1];//.remove('\n');
    return true;
}

bool MainWindow::setConfig(QString name, QString data) {
    QStringList spl;
    for(int cnt = 0; cnt < config.count(); cnt ++) {
        spl = config[cnt].split("=");
        if(!spl[0].compare(name)) {
            config[cnt].replace(spl[0].length() + 1, spl[1].length(), data);
        }
    }
    return true;
}

bool MainWindow::loadConfig() {
    QFile file(homePath + "/.lyra/config.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if(file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QStringList settingsList = QStringList({"wall-name=lyra1_testnet", "cli-path=" + QDir::homePath() + "/.lyra/cli/lira.dll"});
            for(QString line : settingsList) {
                file.write((line + "\n").toUtf8());
            }
        } else {
            dbg("ERROR 'Load config': Creating config file.");
            return false;
        }
    }
    file.seek(0);
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        config.append(line);
    }
    file.close();
    return true;
}

bool MainWindow::saveConfig() {
    QFile file(homePath + "/.lyra/config.txt");
    if(file.remove()) {
        if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            for(QString line : config) {
                QString tmp = line + "\n";
                file.write(tmp.toUtf8());
            }
        } else {
            dbg("ERROR 'Save config': Creating config file.");
            return false;
        }
    } else {
        dbg("ERROR 'Save config': Remove old settings.");
        return false;
    }
    return true;
}

void MainWindow::updaterev_line() {
    QString tmp = "";
    do {
        tmp = QString::fromLocal8Bit(lyraWalletProcess.readLine());
        if(tmp.length() != 0) {
            lyraWallLineResponse.append(tmp);
            //dbg(tmp);
        }
    }while(tmp.length() != 0);
}

void MainWindow::on_hystoryListView_doubleClicked(const QModelIndex &index)
{
    QStandardItem *item = modelHistory->item(index.row());
    QIcon ico = item->icon();
    historyWindow win(this, history.at(index.row()), "Transaction nr " + QString::number(modelHistory->rowCount() - index.row()) + " details:", &ico);
    win.exec();
}

void MainWindow::on_actionNew_Wallet_triggered()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    namePassWindow *createWindow = new namePassWindow();
    createWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    createWindow->exec();
    if(createWindow->getOk()) {
        walletName = createWindow->getWalletName();
        QString password = createWindow->getPassword();
        delete createWindow;
        initProgress("Creating new wallet in progress...");
        if(createWallet(walletName, password)) {
            dbg("INFO 'New wallet': Create new wallet successful.");
        } else {
            dbg("ERROR 'New wallet': Create new wallet error.");
        }
        deleteProgress();
    } else {
        dbg("INFO 'New wallet': Entering wallet name and password for creating new canceled.");
    }
    cliBusySemaphore = false;
}

void MainWindow::on_actionLoad_Wallet_triggered()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    nameWindow *loadWindow = new nameWindow();
    loadWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    loadWindow->exec();
    if(loadWindow->getOk()) {
        walletName = loadWindow->getName();
        delete loadWindow;
        password *passwordWindow = new password();
        passwordWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        passwordWindow->exec();
        if(passwordWindow->getAccepted()) {
            QString pass = passwordWindow->getPass();
            delete passwordWindow;
            initProgress("Loading wallet in progress...");
            if(openWallet(walletName, pass)) {
                dbg("INFO 'Load wallet': Open wallet successful.");
            } else {
                dbg("ERROR'Load wallet': Open wallet error.");
            }
            deleteProgress();
        } else {
            dbg("INFO 'Load wallet': Entering wallet password for load canceled.");
        }
    } else {
        dbg("INFO 'Load wallet': Entering wallet name for load canceled.");
    }
    cliBusySemaphore = false;
}


void MainWindow::on_actionSave_Wallet_triggered()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    cliBusySemaphore = false;
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionReceive_triggered()
{
    on_receivePushButton_clicked();
}

void MainWindow::on_actionSend_triggered()
{
    on_sendPushButton_clicked();
}

void MainWindow::on_actionVote_For_triggered()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    idWindow receiveWindow(this, "Vote for", nullptr);
    receiveWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    receiveWindow.exec();
    if(receiveWindow.getOk()) {
        voteFor(receiveWindow.getId());
    } else {
        dbg("INFO 'Wote for': Entering wallet id for load canceled.");
    }
    cliBusySemaphore = false;
}

void MainWindow::on_actionShow_voted_for_triggered()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    if(lyraWalletProcess.state() == QProcess::Running) {
        idWindow receiveWindow(this, "You voted for", myVotedId);
        receiveWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        receiveWindow.exec();
    } else {
        dbg("INFO 'Show vote for': No wallet opened.");
    }
    cliBusySemaphore = false;
}

void MainWindow::on_actionRecover_from_priv_key_triggered()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    namePassWindow *recoverWindow = new namePassWindow();
    recoverWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    recoverWindow->exec();
    if(recoverWindow->getOk()) {
        walletName = recoverWindow->getWalletName();
        QString password = recoverWindow->getPassword();
        delete recoverWindow;
        pKeyWindow *privateKeyWindow = new pKeyWindow(this, nullptr);
        privateKeyWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        privateKeyWindow->exec();
        if(privateKeyWindow->getOk()) {
            QString key = privateKeyWindow->getPrivate();
            delete privateKeyWindow;
            initProgress("Recovering from private key in progress...");
            if(recoverWallet(walletName, key, password)) {
                dbg("INFO 'Recover wallet': Recover wallet successful.");
            } else {
                dbg("ERROR 'Recover wallet': Recover wallet error.");
            }
            deleteProgress();
        } else {
            dbg("INFO 'Recover wallet': Entering wallet private key for recovery canceled.");
        }
    } else {
        dbg("INFO 'Recover wallet': Entering wallet name and password for recovery canceled.");
    }
    cliBusySemaphore = false;
}

void MainWindow::on_actionShow_private_key_triggered()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    QString key;
    if(showPrivateKey(&key)) {
        pKeyWindow *privateKeyWindow = new pKeyWindow(this, key);
        privateKeyWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        privateKeyWindow->exec();
        delete privateKeyWindow;
    }
    cliBusySemaphore = false;
}

void MainWindow::on_actionDevNet_triggered(bool checked)
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    if(checked && network.compare("devnet")) {
        ui->actionMainNet->setChecked(false);
        ui->actionTestNet->setChecked(false);
        exitCli();
        fee = 0;
        myId = "";
        myVotedId = "";
        numberOfBlocks = 0;
        myBalance = 0.0;
        network = "devnet";
    } else {
        ui->actionDevNet->setChecked(true);
    }
    setWinTitle();
    cliBusySemaphore = false;
}

void MainWindow::on_actionTestNet_triggered(bool checked)
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    if(checked && network.compare("testnet")) {
        ui->actionMainNet->setChecked(false);
        ui->actionDevNet->setChecked(false);
        exitCli();
        fee = 0;
        myId = "";
        myVotedId = "";
        numberOfBlocks = 0;
        myBalance = 0.0;
        network = "testnet";
    } else {
        ui->actionTestNet->setChecked(true);
    }
    setWinTitle();
    cliBusySemaphore = false;
}

void MainWindow::on_actionMainNet_triggered(bool checked)
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    if(checked && network.compare("mainnet")) {
        ui->actionTestNet->setChecked(false);
        ui->actionDevNet->setChecked(false);
        exitCli();
        fee = 0;
        myId = "";
        myVotedId = "";
        numberOfBlocks = 0;
        myBalance = 0.0;
        network = "mainnet";
    } else {
        ui->actionMainNet->setChecked(true);
    }
    setWinTitle();
    cliBusySemaphore = false;
}

void MainWindow::on_actionDebug_triggered()
{
    dbgWindow.show();
}

void MainWindow::on_actionAbout_triggered()
{
    aboutWin.show();
}

void MainWindow::on_receivePushButton_clicked()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    if(showId(&myId)) {
        idWindow receiveWindow(this, "Receive", myId);
        receiveWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        receiveWindow.exec();
    }
    cliBusySemaphore = false;
}

void MainWindow::on_sendPushButton_clicked()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    send sendWindow(this, myBalance);
    sendWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    sendWindow.exec();
    if(sendWindow.getSend()) {
        initProgress("Operation in progress...");
        sendCoins(sendWindow.getId(), sendWindow.getValue());
        deleteProgress();
    }
    cliBusySemaphore = false;
}

void MainWindow::on_syncPushButton_clicked()
{
    if(cliBusySemaphore) {
        return;
    }
    cliBusySemaphore = true;
    initProgress("Sinchronization in progress...");
    syncAccount();
    deleteProgress();
    cliBusySemaphore = false;
}



