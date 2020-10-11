#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QDir>
#include <QTimer>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QProgressDialog>

#include "debug.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool expectResponse(QString *command, QString expectedResponse, QStringList *receivedData, int timeout);
    bool checkNetSdk();
    bool installSdk();
    bool installLyraCli();
    void setWinTitle();
    bool getConfig(QString name, QString &response);
    bool setConfig(QString name, QString data);
    bool loadConfig();
    bool saveConfig();
    bool createWallet(QString name, QString pass);
    bool recoverWallet(QString name, QString key, QString pass);
    bool openWallet(QString wallet, QString pass);
    bool sendCoins(QString id, double value);
    bool readHistory();
    bool syncAccount();
    void exitCli();
    void initProgress();
    void deleteProgress();

    QProcess lyraWalletProcess;
    QString homePath;
    QStringList config;
    double fee;
    QString myId;
    QString myVotedId;
    int numberOfBlocks;
    double myBalance;
    QString err;
    QStringList lyraWallLineResponse;
    QTimer timerInitialize;
    QTimer timerLoadWalletStart;
    QString network;
    QString execPath;

    debug *debugWindow;
    QProgressDialog *loadWalletProgress;

private slots:
    void loadWalletAtStart();

    void on_receivePushButton_clicked();
    void on_sendPushButton_clicked();
    void on_syncPushButton_clicked();

    void updaterev_line();
    void on_actionNew_Wallet_triggered();
    void on_actionLoad_Wallet_triggered();
    void on_actionMainNet_triggered(bool checked);
    void on_actionTestNet_triggered(bool checked);
    void on_actionDevNet_triggered(bool checked);
    void on_actionRecover_from_priv_key_triggered();
    void on_actionAbout_triggered();
    void on_actionDebug_triggered();
    void on_actionExit_triggered();
};
#endif // MAINWINDOW_H
