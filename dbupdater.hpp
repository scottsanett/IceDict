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

#include "dbtransformer.hpp"
#include "quazip/0.9.1/include/quazip/JlCompress.h"


class DBDownloaderHelper: public QObject
{
    Q_OBJECT

public:
    explicit DBDownloaderHelper(QNetworkAccessManager *, QObject * parent = nullptr);
    virtual ~DBDownloaderHelper();
    QByteArray downloadedData() const;
    void proceed(QUrl);

signals:
    void downloaded(int);

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    QUrl DBUrl;
    QNetworkAccessManager * m_WebCtrl;
    QByteArray m_DownloadedData;
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

private slots:
    void processFile(int);

private:
    DBDownloaderHelper * DBCtrl;
    DBTransformer * DBTransformCtrl;

    const char * inputFileName = ".wordlist";
    const char * extractFileName = "SHsnid.csv";
};

#endif // DBDOWNLOADER_HPP
