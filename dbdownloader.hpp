#ifndef DBDOWNLOADER_HPP
#define DBDOWNLOADER_HPP

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QThread>
//#include <QDesktopWidget>

#include "dbtransformer.hpp"
#include "quazip-1.3/quazip/JlCompress.h"

class DBDownloaderHelper: public QObject
{
    Q_OBJECT

public:
    explicit DBDownloaderHelper(QObject * parent = nullptr);
    virtual ~DBDownloaderHelper();
    QByteArray downloadedData() const;
    void proceed();
//    QNetworkAccessManager * getWebCtrl();

signals:
    void downloaded(int);
    void updateStatus(QString const &);
    void signal_ShowProgress();
    void signal_UpdateProgress(qint64, qint64);
    void signal_HideProgress();

private slots:
    void fileDownloaded(QNetworkReply* pReply);
    void slot_UpdateProgress(qint64, qint64);

private:
    QString appDataLocation;
    QUrl DBUrl;
    QNetworkAccessManager * m_WebCtrl;
    QNetworkReply * m_NetworkReply;
    QByteArray m_DownloadedData;
    const char * BINUrl = "https://bin.arnastofnun.is/django/api/nidurhal/?file=SHsnid.csv.zip";
};


class DBDownloader : public QObject
{
    Q_OBJECT

public:
    explicit DBDownloader(DBDownloaderHelper *, QObject * parent = nullptr);
    virtual ~DBDownloader() {}
    void cleanUp();

signals:
    void DBInitializationComplete();
    void updateStatus(QString const);
    void cleanedUp();
    void signal_ShowProgress();
    void signal_HideProgress();
    void signal_UpdateProgress(qint64, qint64);

private slots:
    void processFile(int);
    void acceptUpdate(QString const);
    void slot_ShowProgress();
    void slot_HideProgress();
    void slot_UpdateProgress(qint64, qint64);

private:
    QString appDataLocation;
    DBDownloaderHelper * DBCtrl;
    DBTransformer * DBTransformCtrl;

    const char * inputFileName = ".wordlist";
    const char * extractFileName = "SHsnid.csv";
};

#endif // DBDOWNLOADER_HPP
