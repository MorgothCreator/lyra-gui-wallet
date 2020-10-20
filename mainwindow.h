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
#include <QStandardItemModel>

#include "debugwindow.h"
#include "aboutwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void dbg(QString data);
    void dbg(QStringList data);

private:
    Ui::MainWindow *ui;
    bool expectResponse(QString *command, QString expectedResponse, QStringList *receivedData, int timeout);
    bool installSdk();
    bool checkNetSdk();
    bool installLyraCli();
    bool checkLyraCli();
    bool installSsl();
    bool installCppRedistForSsl();
    void setWinTitle();
    bool getConfig(QString name, QString &response);
    bool setConfig(QString name, QString data);
    bool loadConfig();
    bool saveConfig();
    bool createWallet(QString name, QString pass);
    bool recoverWallet(QString name, QString key, QString pass);
    bool openWallet(QString name, QString pass);
    bool sendCoins(QString id, double value);
    bool showPrivateKey(QString *key);
    bool showId(QString *id);
    bool voteFor(QString id);
    bool showVotedFor();
    void clearHistory();
    bool readHistory(bool update);
    bool syncAccount();
    void exitCli();
    void initProgress(QString title);
    void deleteProgress();

    QProcess lyraWalletProcess;
    QString homePath;
    QStringList config;
    QString walletName;
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

    bool cliBusySemaphore;

    debugWindow dbgWindow;
    QProgressDialog *loadWalletProgress;

    aboutWindow aboutWin;

    QStandardItemModel *modelHistory;
    QStringList history;
    QIcon *attentionIco;
    QIcon *newIco;
    QIcon *sendIco;
    QIcon *receiveIco;
    double lastAmount;

private slots:
    void loadAtStart();

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
    void on_actionShow_private_key_triggered();
    void on_actionVote_For_triggered();
    void on_actionReceive_triggered();
    void on_actionSend_triggered();
    void on_actionShow_voted_for_triggered();
    void on_hystoryListView_doubleClicked(const QModelIndex &index);
    void on_actionSave_Wallet_triggered();
};
#endif // MAINWINDOW_H
