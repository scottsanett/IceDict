#include "PageDownloader.hpp"

PageDownloader::PageDownloader(QNetworkAccessManager * n, QObject * parent): QObject(parent) {
    WebControl = n;
}

PageDownloader::~PageDownloader() {}

void PageDownloader::DownloadPage(QUrl fileUrl) {
    QNetworkRequest request(fileUrl);
    auto pReply = WebControl->get(request);

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
    timer.start(10000);
    loop.exec();

    if (timer.isActive()) {
        timer.stop();
        if (pReply->error() > 0) {
            emit connectionError();
        }
        else {
            auto value = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (value >= 200 && value < 300) {
                DownloadedData = pReply->readAll();
                pReply->deleteLater();
                emit downloaded();
            }
        }
    }
    else {
        disconnect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
        pReply->abort();
        emit timeoutError();
    }
}

void PageDownloader::pageDownloaded(QNetworkReply *pReply) {
    DownloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded();
}

QByteArray PageDownloader::downloadedData() const {
    return DownloadedData;
}
