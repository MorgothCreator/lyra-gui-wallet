#include <QtCore>

#include <QMessageBox>
#include <QThread>
#include <QApplication>
#include <QThread>
#include "httpdownload.h"
#include "def.h"

httpDownload::httpDownload(QWidget *parent)
{
    this->parent = parent;
}

httpDownload::~httpDownload() {

}

bool httpDownload::download(QString progressTitle, QString *filePath, QString *sUrl) {
    file = nullptr;
    progressDialog = nullptr;
    if (!QFile::exists(*filePath)) {
        file = new QFile(*filePath);
        if (!file->open(QIODevice::WriteOnly)) {
            QMessageBox::information(parent, tr("HTTP"),
                          tr("Unable to save the file %1: %2.")
                          .arg(*filePath).arg(file->errorString()));
            delete file;
            file = 0;
            return false;
        }
        httpRequestAborted = false;
        progressDialog = new QProgressDialog(this);
        progressDialog->setWindowTitle(tr(progressTitle.toUtf8()));
        progressDialog->setLabelText(tr("Downloading %1.").arg(*sUrl));
        QIcon pllIcon = QIcon(QCoreApplication::applicationDirPath() + WINDOW_ICO_SMALL);
        progressDialog->setWindowIcon(pllIcon);
        connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));

        manager = new QNetworkAccessManager(parent);
        url.setUrl(*sUrl);
        finished = false;
        isGetSize = false;
        requestSize = false;
        eventLoop = new QEventLoop(parent);
        startRequest(url);
        eventLoop->exec();
        if(!httpRequestAborted) {
            return true;
        }
    }
    return false;
}

bool httpDownload::getSizeEqual(QString *filePath, QString *sUrl) {
    file = nullptr;
    progressDialog = nullptr;
    httpRequestAborted = false;
    redirected = false;
    file = nullptr;
    size = -1;
    progressDialog = nullptr;

    isRedirected = checkRedirect(sUrl);
    manager = new QNetworkAccessManager(parent);
    url.setUrl(*sUrl);
    finished = false;
    isGetSize = false;
    requestSize = true;
    eventLoop = new QEventLoop(parent);
    startRequest(url);
    eventLoop->exec();
    QFile f(*filePath);
    qint64 rSize = size;
    qint64 fSize = f.size();
    if(fSize != rSize) {
        if(QFile::exists(*filePath)) {
            QFile::remove(*filePath);
        }
        return false;
    } else {
        return true;
    }
}

bool httpDownload::checkRedirect(QString *sUrl) {
    url.setUrl(*sUrl);
    file = nullptr;
    progressDialog = nullptr;
    manager = new QNetworkAccessManager(parent);
    manager->setStrictTransportSecurityEnabled(false);
    reply = manager->get(QNetworkRequest(url));
    eventLoop = new QEventLoop(parent);
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(quitLoop()));
    eventLoop->exec();

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(!redirectionTarget.isNull()) {
        reply->abort();
        reply->deleteLater();
        manager->deleteLater();
        return true;
    }
    reply->abort();
    reply->deleteLater();
    manager->deleteLater();
    return false;
}

void httpDownload::quitLoop() {
    emit eventLoop->quit();
}

void httpDownload::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void httpDownload::updateDownloadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;

    //size = totalBytes;

    if(progressDialog) {
        progressDialog->setMaximum(totalBytes);
        progressDialog->setValue(bytesRead);
    }
}

// During the download progress, it can be canceled
void httpDownload::cancelDownload()
{
    httpRequestAborted = true;
    reply->abort();
}

// When download finished or canceled, this will be called
void httpDownload::httpDownloadFinished()
{
    // when canceled
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        reply->deleteLater();
        if(progressDialog) {
            progressDialog->hide();
        }
        finished = true;
        emit eventLoop->quit();
        return;
    }

    // download finished normally
    if(progressDialog) {
        progressDialog->hide();
    }
    if (file) {
        file->flush();
        file->close();
    }

    // get redirection url
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        if (file) {
            file->remove();
        }
        if(!requestSize) {
        QMessageBox::information(this, tr("HTTPS"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
        }
        //ui->downloadButton->setEnabled(true);
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        redirected = true;
        /*if (QMessageBox::question(this, tr("HTTPS"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)*/ {
            url = newUrl;
            reply->deleteLater();
            if (file) {
                file->open(QIODevice::WriteOnly);
                file->resize(0);
            }
            isGetSize = true;
            startRequest(url);
            return;
        }
    } else {
        //QString fileName = QFileInfo(QUrl(ui->urlEdit->text()).path()).fileName();
        //ui->statusLabel->setText(tr("Downloaded %1 to %2.").arg(fileName).arg(QDir::currentPath()));
        //ui->downloadButton->setEnabled(true);
    }

    reply->deleteLater();
    reply = 0;
    if (file) {
        delete file;
    }
    file = 0;
    manager = 0;
    finished = true;
    emit eventLoop->quit();
}

void httpDownload::fileSize() {
    size = reply->header(QNetworkRequest::ContentLengthHeader).toString().toLongLong();
    if((isGetSize && requestSize) || (!isRedirected && requestSize)) {
        reply->abort();
        emit eventLoop->quit();
    }
}

// This will be called when download button is clicked
void httpDownload::startRequest(QUrl url) {
    if(progressDialog) {
        progressDialog->show();
    }
//https://github-production-release-asset-2e65be.s3.amazonaws.com/274731863/f9b1e900-0aa5-11eb-98a8-9039778d56ff?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20201013%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20201013T120103Z&X-Amz-Expires=300&X-Amz-Signature=72be20ad204551e5bf9f4515b2dbe2a65ac4087b071c139c11239fdd633c0623&X-Amz-SignedHeaders=host&actor_id=0&key_id=0&repo_id=274731863&response-content-disposition=attachment%3B%20filename%3Dlyra.permissionless-1.7.8.0.tar.bz2&response-content-type=application%2Foctet-stream
    //finished = false;
    // get() method posts a request
    // to obtain the contents of the target request
    // and returns a new QNetworkReply object
    // opened for reading which emits
    // the readyRead() signal whenever new data arrives.
    //if(!redirected) {
    //}
    reply = manager->get(QNetworkRequest(url));

    // Whenever more data is received from the network,
    // this readyRead() signal is emitted
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));

    // Also, downloadProgress() signal is emitted when data is received
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDownloadProgress(qint64,qint64)));

    // This signal is emitted when the reply has finished processing.
    // After this signal is emitted,
    // there will be no more updates to the reply's data or metadata.
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpDownloadFinished()));

    connect(reply, SIGNAL(metaDataChanged()), this, SLOT(fileSize()));
}

