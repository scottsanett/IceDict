#include "dbupdater.hpp"


DBDownloaderHelper::DBDownloaderHelper(QNetworkAccessManager * n, QObject * parent): QObject(parent)
{   
    m_WebCtrl = n;
    connect(m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
            this, SLOT (fileDownloaded(QNetworkReply*)));
}

DBDownloaderHelper::~DBDownloaderHelper() { }

void DBDownloaderHelper::proceed(QUrl DBUrl) {
    if (QFileInfo(".wordlist").exists()) {
        qDebug() << "Database file already exists";
        emit downloaded(-1);
    }
    else {
        qDebug() << "Initiating download...";
        QNetworkRequest request(DBUrl);
        m_WebCtrl->get(request);
    }
}

void DBDownloaderHelper::fileDownloaded(QNetworkReply* pReply) {
    m_DownloadedData = pReply->readAll();
    qDebug() << "Download completed";
    //emit a signal
    pReply->deleteLater();
    emit downloaded(0);
}

QByteArray DBDownloaderHelper::downloadedData() const {
    return m_DownloadedData;
}

DBDownloader::DBDownloader(DBDownloaderHelper * helper, QObject * parent): QObject(parent) {
    DBCtrl = helper;
}

void DBDownloader::processFile(int status) {
    if (status == 0) {
        QSaveFile file(inputFileName);
        file.open(QIODevice::WriteOnly);
        file.write(DBCtrl->downloadedData());
        file.commit();
    }
    qDebug() << "Initiating transformation";
    auto && fileName = JlCompress::extractFile(inputFileName, extractFileName);
    DBTransformCtrl = new DBTransformer(this);
    DBTransformCtrl->transform(fileName.toStdString());
    emit DBInitializationComplete();
}

void DBDownloader::cleanUp() {
    QFile input(inputFileName), extract(extractFileName);
    input.remove(); extract.remove();
}

