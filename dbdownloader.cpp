#include "dbdownloader.hpp"


DBDownloaderHelper::DBDownloaderHelper(QObject * parent): QObject(parent)
{   
    appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

DBDownloaderHelper::~DBDownloaderHelper() { }


void DBDownloaderHelper::proceed() {
    appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir::setCurrent(appDataLocation);

    auto DBUrl = QUrl(BINUrl);
    m_WebCtrl = new QNetworkAccessManager(this);
    connect(m_WebCtrl, SIGNAL (finished(QNetworkReply*)),
            this, SLOT (fileDownloaded(QNetworkReply*)));

    if (QFileInfo(".wordlist").exists()) {
        qDebug() << "Database source already exists";
        emit updateStatus("Database source already exists.");
        emit downloaded(-1);
    }
    else {
        qDebug() << "Initiating download";
        emit updateStatus("Initiating download...");
        QNetworkRequest request(DBUrl);
        m_NetworkReply = m_WebCtrl->get(request);
        connect(m_NetworkReply, SIGNAL(downloadProgress(qint64, qint64)),
                this, SLOT(slot_UpdateProgress(qint64, qint64)));
        emit signal_ShowProgress();
    }
}

void DBDownloaderHelper::slot_UpdateProgress(qint64 a, qint64 b) {
    emit signal_UpdateProgress(a, b);
}

/*
QNetworkAccessManager * DBDownloaderHelper::getWebCtrl() {
    return m_WebCtrl;
}
*/

void DBDownloaderHelper::fileDownloaded(QNetworkReply* pReply) {
    m_DownloadedData = pReply->readAll();
    qDebug() << "Download completed";
    emit updateStatus("Download completed.");
    //emit a signal
    pReply->deleteLater();
    emit signal_HideProgress();
    emit downloaded(0);
}

QByteArray DBDownloaderHelper::downloadedData() const {
    return m_DownloadedData;
}

DBDownloader::DBDownloader(DBDownloaderHelper * helper, QObject * parent): QObject(parent) {
    appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
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
    emit updateStatus("Initiating transformation.");
    auto && fileName = JlCompress::extractFile(inputFileName, extractFileName);
    DBTransformCtrl = new DBTransformer(this);
    connect(DBTransformCtrl, SIGNAL(updateStatus(QString const)), this, SLOT(acceptUpdate(QString const)));
    connect(DBTransformCtrl, SIGNAL(signal_ShowProgress()), this, SLOT(slot_ShowProgress()));
    connect(DBTransformCtrl, SIGNAL(signal_HideProgress()), this, SLOT(slot_HideProgress()));
    connect(DBTransformCtrl, SIGNAL(signal_UpdateProgress(qint64, qint64)), this, SLOT(slot_UpdateProgress(qint64, qint64)));
    DBTransformCtrl->transform(fileName.toStdString());
    DBTransformCtrl->deleteLater();
    DBTransformCtrl = nullptr;
    cleanUp();
}

void DBDownloader::cleanUp() {
    QDir::setCurrent(appDataLocation);
    QFile input(inputFileName), extract(extractFileName);
    input.remove(); extract.remove();
    emit cleanedUp();
}

void DBDownloader::acceptUpdate(QString const str) {
    emit updateStatus(str);
}

void DBDownloader::slot_ShowProgress() {
    emit signal_ShowProgress();
}

void DBDownloader::slot_HideProgress() {
    emit signal_HideProgress();
}

void DBDownloader::slot_UpdateProgress(qint64 a, qint64 b) {
    emit signal_UpdateProgress(a, b);
}
