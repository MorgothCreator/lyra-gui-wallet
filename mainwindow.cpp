#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QThread>
#include <QMessageBox>
#include <QSettings>

#include "showprivatekey.h"
#include "receive.h"
#include "send.h"
#include "password.h"
#include "create.h"
#include "load.h"
#include "about.h"

#include "httpdownload.h"

#include "def.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    execPath = QCoreApplication::applicationDirPath();
    QIcon pllIcon = QIcon(execPath + WINDOW_ICO_SMALL);
    this->setWindowIcon(pllIcon);

    debugWindow = new debug(this);



    fee = 0;
    myId = "";
    myVotedId = "";
    numberOfBlocks = 0;
    myBalance = 0.0;

    if(ui->actionMainNet->isChecked()) {
        network = "mainnet";
    } else if(ui->actionTestNet->isChecked()) {
        network = "testnet";
    } else if(ui->actionDevNet->isChecked()) {
        network = "devnet";
    }

    setWinTitle();

    homePath = QDir::homePath();
    loadConfig();

    timerLoadWalletStart.setInterval(250);
    connect(&timerLoadWalletStart, SIGNAL(timeout()), this, SLOT(loadAtStart()));
    timerLoadWalletStart.start();


}

MainWindow::~MainWindow()
{
    if(lyraWalletProcess.state() == QProcess::Running) {
        exitCli();
    }
    delete debugWindow;
    delete ui;
}

void MainWindow::loadAtStart() {
    timerLoadWalletStart.stop();
    installDotNetSdkForSsl();
    installSsl();
    checkNetSdk();
    /* Force install, only for debug. */
    //installSdk();
    installLyraCli();

    on_actionLoad_Wallet_triggered();
}

void MainWindow::dbg(QStringList data) {
    if(data.count() != 0) {
        err = data[data.count() - 1];
    }
    debugWindow->append(data);
    qDebug() << data << Qt::endl;
}

void MainWindow::dbg(QString data) {
    err = data;
    debugWindow->append(data);
    qDebug() << data << Qt::endl;
}

void MainWindow::initProgress() {
    loadWalletProgress = new QProgressDialog(this);
    loadWalletProgress->setMaximum(0);
    loadWalletProgress->setValue(0);
    loadWalletProgress->setWindowTitle(tr("Operation in progress..."));
    QIcon pllIcon = QIcon(execPath + WINDOW_ICO_SMALL);
    loadWalletProgress->setWindowIcon(pllIcon);
    loadWalletProgress->show();
}

void MainWindow::deleteProgress() {
    delete loadWalletProgress;
}


bool MainWindow::installSdk() {
    dbg("ERROR: NET SDK 3.1.402 not found.");
    dbg("INFO: We will download and install NET SDK 3.1.402.");

    httpDownload sdkDownload(this);
    if(!sdkDownload.getSizeEqual(new QString(DEPENDENCY_DIR + DOT_NET_CORE_NAME), new QString("https://download.visualstudio.microsoft.com/download/pr/9706378b-f244-48a6-8cec-68a19a8b1678/1f90fd18eb892cbb0bf75d9cff377ccb/dotnet-sdk-3.1.402-win-x64.exe"))) {
        if(!sdkDownload.download(QString("Download " + QString(DOT_NET_CORE_NAME)), new QString(DEPENDENCY_DIR + DOT_NET_CORE_NAME), new QString("https://download.visualstudio.microsoft.com/download/pr/9706378b-f244-48a6-8cec-68a19a8b1678/1f90fd18eb892cbb0bf75d9cff377ccb/dotnet-sdk-3.1.402-win-x64.exe"))) {
            QMessageBox::critical(this, tr("ERROR"),
                                     tr("LYRA CLI wallet can not run without\n\r .NET Core 3.1.402.\n\r\n\r This GUI will start for demonstration only."));
            return false;
        }
    }

    QString program = DEPENDENCY_DIR + DOT_NET_CORE_NAME;
    QStringList arguments;
    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);
    if(lyraWalletProcess.waitForStarted(5000)) {
        lyraWalletProcess.waitForFinished();
        /* Commenting, it freezes here. */
        //while(lyraWalletProcess.state() == QProcess::Running);
        lyraWalletProcess.close();
    } else {
        dbg("ERROR: .NET Core installer not starting.");
    }
    return true;
}

bool MainWindow::checkNetSdk() {
    QString programFilesPath = QString::fromUtf8(qgetenv("ProgramW6432"));
    QString program = PROGRAM_FILES_6432 + "dotnet/dotnet.exe";
    QStringList arguments;
    arguments << "--list-sdks";
    QProcess netSdk;
    netSdk.setParent(this);
    netSdk.setReadChannel(QProcess::StandardOutput);
    netSdk.start(program, arguments);

    if(netSdk.waitForStarted(5000)) {
        netSdk.waitForFinished();
        QString sdkCliRead = netSdk.readAll();
        QStringList sdkVersions = sdkCliRead.split("\n");
        sdkVersions.removeAll("");
        for(QString line : sdkVersions) {
            dbg("INFO: NET SDK Version: " + line.remove("\r"));
        }
        if(sdkCliRead.contains("3.1.402")) {
            dbg("INFO: NET SDK 3.1.402 found.");
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
        dbg("INFO: LYRA CLI wallet archive not found.");
        dbg("INFO: Proceed to download LYRA CLI wallet.");
        if(!sdkDownload.download(QString("Download lyra.permissionless-" + QString(LYRA_CLI_VERSION) + ".tar.bz2"), new QString(DEPENDENCY_DIR + QString(LYRA_CLI_VERSION) + ".tar.bz2"), new QString(LYRA_CLI_DLD_ADDR))) {
            dbg("ERROR: Downloading LYRA CLI archive.");
            return false;
        }
    }
    dbg("INFO: Download LYRA CLI wallet succesful.");
    QString program = DEPENDENCY_DIR + "7za.exe";
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
            lyraWalletProcess.waitForFinished();
            while(lyraWalletProcess.state() == QProcess::Running);
            lyraWalletProcess.close();
            QFile fil(DEPENDENCY_DIR + LYRA_CLI_VERSION + ".tar");
            fil.remove();
            dbg("INFO: LYRA CLI wallet extracted.");
        } else {
            dbg("ERROR: 7z not starting.");
            return false;
        }
    } else {
        dbg("ERROR: 7z not starting.");
        return false;
    }
    return true;
}

bool MainWindow::installSsl () {
    /*
     * Look for Open SSL executable in program files, if is not found, install it.
     */
    if(!QFile::exists(PROGRAM_FILES_6432 + "OpenSSL-Win64/bin/openssl.exe")) {
        dbg("INFO: Open SSL not installed on this machine.");
        dbg("INFO: Proced to instal Open SSL...");
        QString program = DEPENDENCY_DIR + "Win64OpenSSL_Light-1_1_1h";
        QStringList arguments;
        lyraWalletProcess.setParent(this);
        lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
        connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
        lyraWalletProcess.start(program, arguments);
        if(lyraWalletProcess.waitForStarted(5000)) {
            dbg("INFO: Open SSL installer started.");
            lyraWalletProcess.waitForFinished();
            while(lyraWalletProcess.state() == QProcess::Running);
            lyraWalletProcess.close();
        } else {
            dbg("ERROR: SSL installer not starting.");
            return false;
        }
    } else {
        dbg("OK: Open SSL already installed on this PC.");
        return false;
    }
    return true;
}

bool MainWindow::installDotNetSdkForSsl () {
    /*
     * Look for Open SSL executable in program files, if is not found, install it.
     */
    dbg("INFO: Seatch for C++ 2017 Redistributable on this machine.");
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\14.0\\VC\\Runtimes\\x64", QSettings::NativeFormat);
    QString cppRedVersion = reg.value("Version").toString();
    if(cppRedVersion.length()) {
        dbg("INFO: C++ 2017 Redistributable " + cppRedVersion + " found.");
    } else {
        dbg("INFO: C++ 2017 Redistributable " + cppRedVersion + " not found.");
    }
    qlonglong cppRedVersionValue = cppRedVersion.replace(".", "").replace("v", "").toLongLong();
    if(cppRedVersionValue >= qlonglong(14162703300)) {
        dbg("INFO: C++ 2017 Redistributable " + cppRedVersion +" found on PC and is newer than the one in this package.");
        return true;
    }

    if(QFile::exists(DEPENDENCY_DIR + "VC_redist.x64.exe")) {
        dbg("INFO: Local C++ 2017 Redistributable installer found");
        QString program = DEPENDENCY_DIR + "VC_redist.x64.exe";
        QStringList arguments;
        lyraWalletProcess.setParent(this);
        lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
        connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
        lyraWalletProcess.start(program, arguments);
        if(lyraWalletProcess.waitForStarted(5000)) {
            dbg("INFO: C++ 2017 Redistributable installer started.");
            lyraWalletProcess.waitForFinished();
            while(lyraWalletProcess.state() == QProcess::Running);
            lyraWalletProcess.close();
        } else {
            dbg("ERROR: SSL installer not starting.");
            return false;
        }
    } else {
        dbg("ERROR: VC_redist.x64.exe installer not found.");
        return false;
    }
    return true;
}

void MainWindow::setWinTitle() {
    this->setWindowTitle(QString::asprintf(MAIN_WINDOW_TITLE, network.toStdString().data()));
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
        exitCli();
    }
    QString program = "C:\\Program Files\\dotnet\\dotnet.exe";
    QStringList arguments;
    arguments << execPath + "\\lyra\\cli\\lyra.dll" << "--networkid" << network << "-p" << "webapi";

    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);

    loadWalletProgress->setMaximum(5);
    loadWalletProgress->setValue(0);

    if(lyraWalletProcess.waitForStarted(5000))
    {
        dbg("INFO: LYRA CLI wallet instance started.");
        loadWalletProgress->setValue(1);
        if(expectResponse(nullptr, QString("Press Enter for default account"), nullptr, 2000)) {
            loadWalletProgress->setValue(2);
            dbg("INFO: LYRA CLI wallet responded as expected.");
            QString set = name + "\n";
            if(expectResponse(&set, QString("Local account data not found."), nullptr, 2000)) {
                loadWalletProgress->setValue(3);
                set = "Y\n";
                if(expectResponse(&set, QString("Please input a password:"), nullptr, 2000)) {
                    loadWalletProgress->setValue(4);
                    set = pass + "\n";
                    QStringList rec;
                    if(expectResponse(&set, QString("Balance:"), &rec, 5000)) {
                        dbg("INFO: " + name + " LYRA wallet successfully oppened.");
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
                    } else {
                        dbg("ERROR: Unknown error.");
                        return false;
                    }
                } else {
                    dbg("ERROR: Invalid password format/ wrong password.");
                    return false;
                }
            } else {
                dbg("ERROR: Wallet already exist.");
                return false;
            }
        } else {
            dbg("ERROR: Unrecognized response from cli.");
            return false;
        }
    } else {
        dbg("ERROR: Wallet cli not found at given location.");
        return false;
    }
    return true;
}

bool MainWindow::recoverWallet(QString name, QString key, QString pass) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        exitCli();
    }
    QString program = "C:\\Program Files\\dotnet\\dotnet.exe";
    QStringList arguments;
    arguments << execPath + "\\lyra\\cli\\lyra.dll" << "--networkid" << network << "-p" << "webapi";

    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);

    loadWalletProgress->setMaximum(6);
    loadWalletProgress->setValue(0);
    if(lyraWalletProcess.waitForStarted(5000))
    {
        dbg("INFO: LYRA CLI wallet instance started.");
        loadWalletProgress->setValue(1);
        if(expectResponse(nullptr, QString("Press Enter for default account"), nullptr, 2000)) {
            loadWalletProgress->setValue(2);
            dbg("INFO: LYRA wallet responded as expected.");
            QString set = name + "\n";
            if(expectResponse(&set, QString("Local account data not found."), nullptr, 2000)) {
                loadWalletProgress->setValue(3);
                set = "n\n";
                if(expectResponse(&set, QString("Please enter private key to restore account"), nullptr, 2000)) {
                    loadWalletProgress->setValue(4);
                    set = key + "\n";
                    if(expectResponse(&set, QString("Please input a password"), nullptr, 2000)) {
                        loadWalletProgress->setValue(5);
                        set = pass + "\n";
                        QStringList rec;
                        if(expectResponse(&set, QString("Balance:"), &rec, 5000)) {
                            dbg("INFO: " + name + " LYRA wallet successfully oppened.");
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
                            syncAccount();
                        } else {
                            dbg("ERROR: Unknown error.");
                            return false;
                        }
                    } else {
                        dbg("ERROR: Invalid password format/ wrong password.");
                        return false;
                    }
                } else {
                    dbg("ERROR: Invalid private key.");
                    return false;
                }
            } else {
                dbg("ERROR: Wallet already exist.");
                return false;
            }
        } else {
            dbg("ERROR: Unrecognized response from cli.");
            return false;
        }
    } else {
        dbg("ERROR: Wallet cli not found at given location.");
        return false;
    }
    return true;
}

bool MainWindow::openWallet(QString name, QString pass) {
    if(lyraWalletProcess.state() == QProcess::Running) {
        exitCli();
    }
    QString program = "C:\\Program Files\\dotnet\\dotnet.exe";
    QStringList arguments;
    arguments << execPath + "\\lyra\\cli\\lyra.dll" << "--networkid" << network << "-p" << "webapi";

    lyraWalletProcess.setParent(this);
    lyraWalletProcess.setReadChannel(QProcess::StandardOutput);
    connect(&lyraWalletProcess,SIGNAL(readyReadStandardOutput()) ,this,SLOT(updaterev_line()));
    lyraWalletProcess.start(program, arguments);

    loadWalletProgress->setMaximum(5);
    loadWalletProgress->setValue(0);
    if(lyraWalletProcess.waitForStarted(5000))
    {
        dbg("INFO: LYRA CLI wallet instance started.");
        loadWalletProgress->setValue(1);
        if(expectResponse(nullptr, QString("Press Enter for default account"), nullptr, 2000)) {
            loadWalletProgress->setValue(2);
            dbg("INFO: LYRA wallet responded as expected.");
            QString set;
            if(name == nullptr) {
                getConfig(QString("wall-name"), set);
            } else {
                set = name + "\n";
            }
            if(expectResponse(&set, QString("Please input a password to open wallet"), nullptr, 5000)) {
                loadWalletProgress->setValue(3);
                set = pass + "\n";
                if(expectResponse(&set, QString("Type 'help' to see the list"), nullptr, 2000)) {
                    loadWalletProgress->setValue(4);
                    QStringList rec;
                    set = "";
                    expectResponse(nullptr, set, &rec, 5000);
                    loadWalletProgress->setValue(5);
                    dbg("INFO: " + name + " LYRA wallet successfully oppened.");
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
                } else {
                    dbg("ERROR: Invalid password/ wrong password.");
                    return false;
                }
            } else {
                dbg("ERROR: Invalid wallet name.");
                return false;
            }

        } else {
            dbg("ERROR: Oppening wallet cli.");
            return false;
        }
    }
    else
    {
        dbg("ERROR: Wallet cli not found at given location.");
        return false;
    }
    return true;
}

bool MainWindow::sendCoins(QString id, double value) {
    QString set = "send\n";
    loadWalletProgress->setMaximum(5);
    loadWalletProgress->setValue(0);
    if(expectResponse(&set, QString("Please enter destination account id"), nullptr, 2000)) {
        loadWalletProgress->setValue(1);
        set = id + "\n";
        dbg("INFO: ID Sending:" + set);
        if(expectResponse(&set, QString("Please enter amount"), nullptr, 2000)) {
            loadWalletProgress->setValue(2);
            QLocale sys = QLocale::system();
            set = QString::asprintf("%.8f", value) + "\n";
            if(sys.decimalPoint() == ',') {
                set = set.replace(".", ",");
            }
            dbg("INFO: Coins sending:" + set);
            if(expectResponse(&set, QString("Send Transfer block has been authorized successfully"), nullptr, 10000)) {
                dbg(QString("INFO: Coins send: ") + QString::asprintf("%.8f", value) + QString("LYR, to id: ") + id);
                syncAccount();
            } else {
                dbg("ERROR: Invalid value.");
                return false;
            }
        } else {
            dbg("ERROR: Invalid id.");
            return false;
        }
    } else {
        dbg("ERROR: Unnexpected response from cli.");
    }
    return true;
}

bool MainWindow::syncAccount() {
    QString set = "sync\n";
    loadWalletProgress->setMaximum(2);
    loadWalletProgress->setValue(0);
    dbg("INFO: Sync in progress." + set);
    if(expectResponse(&set, QString("Sync Result: Success"), nullptr, 10000)) {
        loadWalletProgress->setValue(1);
        dbg("INFO: Sync successfull.");
        set = "balance\n";
        QStringList rec;
        if(expectResponse(&set, QString("LYR"), &rec, 5000)) {
            loadWalletProgress->setValue(2);
            dbg("INFO: Parse balance.");
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
        } else {
            dbg("ERROR: Unnexpected response from cli.");
            return false;
        }
    } else {
        dbg("ERROR: Unnexpected response from cli.");
        return false;
    }
    return true;
}

bool MainWindow::readHistory() {
    return true;
}

bool MainWindow::expectResponse(QString *command, QString expectedResponse, QStringList *receivedData, int timeout){
    lyraWallLineResponse.clear();
    if(command != nullptr && command->length() != 0) {
        lyraWalletProcess.write(command->toUtf8());
    }
    int cnt = timeout / 250;
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
        QThread::msleep(250);
        QApplication::processEvents();
        cnt--;
        tmpData.append(lyraWallLineResponse);
        debugWindow->append(lyraWallLineResponse);
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
            dbg("ERROR: LYRA CLI killed.");
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
            dbg("ERROR: Creating config file.");
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
            dbg("ERROR: Creating config file.");
            return false;
        }
    } else {
        dbg("ERROR: Remove old settings.");
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

void MainWindow::on_receivePushButton_clicked()
{
    receive receiveWindow(this, myId);
    receiveWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    receiveWindow.exec();
}

void MainWindow::on_sendPushButton_clicked()
{
    send sendWindow(this, myBalance);
    sendWindow.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    sendWindow.exec();
    if(sendWindow.getSend()) {
        initProgress();
        sendCoins(sendWindow.getId(), sendWindow.getValue());
        deleteProgress();
    }
}

void MainWindow::on_syncPushButton_clicked()
{
    initProgress();
    syncAccount();
    deleteProgress();
}

void MainWindow::on_actionNew_Wallet_triggered()
{
    createNewWallet *createWindow = new createNewWallet();
    createWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    createWindow->exec();
    if(createWindow->getOk()) {
        QString wallName = createWindow->getWalletName();
        QString password = createWindow->getPassword();
        delete createWindow;
        initProgress();
        if(createWallet(wallName, password)) {
            dbg("INFO: Create new wallet successful.");
        } else {
            dbg("ERROR: Create new wallet error.");
        }
        deleteProgress();
    } else {
        dbg("INFO: Entering wallet name and password for creating new canceled.");
    }
}

void MainWindow::on_actionRecover_from_priv_key_triggered()
{
    createNewWallet *createWindow = new createNewWallet();
    createWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    createWindow->exec();
    if(createWindow->getOk()) {
        QString wallName = createWindow->getWalletName();
        QString password = createWindow->getPassword();
        delete createWindow;
        privateKey *privateKeyWindow = new privateKey(this, true);
        privateKeyWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        privateKeyWindow->exec();
        if(privateKeyWindow->getOk()) {
            QString key = privateKeyWindow->getPrivate();
            delete privateKeyWindow;
            initProgress();
            if(recoverWallet(wallName, key, password)) {
                dbg("INFO: Recover wallet successful.");
            } else {
                dbg("ERROR: Recover wallet error.");
            }
            deleteProgress();
        } else {
            dbg("INFO: Entering wallet private key for recovery canceled.");
        }
    } else {
        dbg("INFO: Entering wallet name and password for recovery canceled.");
    }
}

void MainWindow::on_actionLoad_Wallet_triggered()
{
    loadWallet *loadWindow = new loadWallet();
    loadWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    loadWindow->exec();
    if(loadWindow->getOk()) {
        QString walletName = loadWindow->getName();
        delete loadWindow;
        password *passwordWindow = new password();
        passwordWindow->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        passwordWindow->exec();
        if(passwordWindow->getAccepted()) {
            QString pass = passwordWindow->getPass();
            delete passwordWindow;
            initProgress();
            if(openWallet(walletName, pass)) {
                dbg("INFO: Oppen wallet successful.");
            } else {
                dbg("ERROR: Oppen wallet error.");
            }
            deleteProgress();
        } else {
            dbg("INFO: Entering wallet password for load canceled.");
        }
    } else {
        dbg("INFO: Entering wallet name for load canceled.");
    }
}

void MainWindow::on_actionMainNet_triggered(bool checked)
{
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
}

void MainWindow::on_actionTestNet_triggered(bool checked)
{
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
}

void MainWindow::on_actionDevNet_triggered(bool checked)
{
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
}

void MainWindow::on_actionAbout_triggered()
{
    about aboutWindow;
    aboutWindow.exec();
}


void MainWindow::on_actionDebug_triggered()
{
    debugWindow->show();
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}
