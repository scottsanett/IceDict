#include "PageDownloader.hpp"

PageDownloader::PageDownloader(QObject * parent): QObject(parent) {
    QObject::connect(&WebControl, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(pageDownloaded(QNetworkReply*)));
}

PageDownloader::~PageDownloader() {}

void PageDownloader::DownloadPage(QUrl fileUrl) {
    QNetworkRequest request(fileUrl);
    WebControl.get(request);
}

void PageDownloader::pageDownloaded(QNetworkReply *pReply) {
    DownloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded();
}

QByteArray PageDownloader::downloadedData() const {
    return DownloadedData;
}
