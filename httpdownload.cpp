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
        progressDialog->show();

        manager = new QNetworkAccessManager(parent);
        url.setUrl(*sUrl);
        finished = false;
        startRequest(url);
        while(!finished) {
            QApplication::processEvents();
        }
    }
    return false;
}

bool httpDownload::getSizeEqual(QString *filePath, QString *sUrl) {
    httpRequestAborted = false;
    redirected = false;
    file = nullptr;
    qint64 sizeRedirected = -1;
    size = -1;
    progressDialog = nullptr;

    manager = new QNetworkAccessManager(parent);
    url.setUrl(*sUrl);
    finished = false;
    startRequest(url);
    while(!finished) {
        QApplication::processEvents();
        if(size != -1) {
            break;
        }
    }
    sizeRedirected = size;

    for (int cnt = 0; cnt < 5000; cnt++) {
        QThread::msleep(1);
        QApplication::processEvents();
        if(!redirected || sizeRedirected != size) {
            break;
        }
    }

    httpRequestAborted = true;
    reply->abort();

    QFile f(*filePath);
    int rSize = size;
    int fSize = f.size();
    if(fSize != rSize) {
        return false;
    } else {
        return true;
    }
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

    size = totalBytes;

    if(progressDialog) {
        progressDialog->setMaximum(totalBytes);
        progressDialog->setValue(bytesRead);
    }
}

// During the download progress, it can be canceled
void httpDownload::cancelDownload()
{
    //ui->statusLabel->setText(tr("Download canceled."));
    httpRequestAborted = true;
    reply->abort();
    //ui->downloadButton->setEnabled(true);
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
        QMessageBox::information(this, tr("HTTPS"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
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
}

// This will be called when download button is clicked
void httpDownload::startRequest(QUrl url) {
    // get() method posts a request
    // to obtain the contents of the target request
    // and returns a new QNetworkReply object
    // opened for reading which emits
    // the readyRead() signal whenever new data arrives.
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
}

