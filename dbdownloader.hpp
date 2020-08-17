#ifndef DBDOWNLOADER_HPP
#define DBDOWNLOADER_HPP

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileinfo>
#include <QSaveFile>
#include <QThread>
#include <QDesktopWidget>

#include "dbtransformer.hpp"
#include "quazip/0.9.1/include/quazip/JlCompress.h"


class DBDownloaderHelper: public QObject
{
    Q_OBJECT

public:
    explicit DBDownloaderHelper(QObject * parent = nullptr);
    virtual ~DBDownloaderHelper();
    QByteArray downloadedData() const;
    void proceed();

signals:
    void downloaded(int);
    void updateStatus(QString const &);

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    QString appDataLocation;
    QUrl DBUrl;
    QNetworkAccessManager * m_WebCtrl;
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

private slots:
    void processFile(int);
    void acceptUpdate(QString const);

private:
    QString appDataLocation;
    DBDownloaderHelper * DBCtrl;
    DBTransformer * DBTransformCtrl;

    const char * inputFileName = ".wordlist";
    const char * extractFileName = "SHsnid.csv";
};

#endif // DBDOWNLOADER_HPP
