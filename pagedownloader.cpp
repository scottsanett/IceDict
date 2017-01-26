#include "pagedownloader.h"

PageDownloader::PageDownloader(QUrl fileUrl, QObject *parent) : QObject(parent)
{
    connect (&WebControl, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(pageDownloaded(QNetworkReply*)));
    QNetworkRequest request(fileUrl);
    WebControl.get(request);
}

PageDownloader::~PageDownloader() {}

void PageDownloader::pageDownloaded(QNetworkReply *pReply) {
    DownloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded();
}

QByteArray PageDownloader::downloadedData() const {
    return DownloadedData;
}
