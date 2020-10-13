#ifndef HTTPDOWNLOAD_H
#define HTTPDOWNLOAD_H

#include <QMainWindow>
#include <QObject>
#include <QFile>
#include <QUrl>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

class httpDownload : public QMainWindow
{
    Q_OBJECT
public:
    httpDownload(QWidget *parent = nullptr);
    ~httpDownload();
    bool download(QString progressTitle = nullptr, QString *filePath = nullptr, QString *sUrl = nullptr);
    bool getSizeEqual(QString *filePath = nullptr, QString *sUrl = nullptr);
    bool checkRedirect(QString *sUrl);

    bool httpRequestAborted;

private:
    void startRequest(QUrl url);

    QWidget *parent;
    QUrl url;
    QProgressDialog *progressDialog;
    QFile *file;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    bool finished;
    bool isGetSize;
    bool requestSize;
    bool chkRedirect;
    bool isRedirected;
    QEventLoop *eventLoop;

    bool getSizeFinished;
    bool redirected;
    qint64 size;
private slots:
    // slot for readyRead() signal
    void httpReadyRead();

    // slot for finished() signal from reply
    void httpDownloadFinished();

    // slot for downloadProgress()
    //void updateDownloadProgress(qint64, qint64);

    void cancelDownload();

    // slot for downloadProgress()
    void updateDownloadProgress(qint64, qint64);

    void fileSize();
    void quitLoop();

};

#endif // HTTPDOWNLOAD_H
