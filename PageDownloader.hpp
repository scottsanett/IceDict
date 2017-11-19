#ifndef PAGEDOWNLOADER_H
#define PAGEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>

class PageDownloader : public QObject
{
    Q_OBJECT
public:
    explicit PageDownloader(QUrl pageUrl, QObject *parent = 0);
    virtual ~PageDownloader();
    QByteArray downloadedData() const;

signals:
    void downloaded();
public slots:

private slots:
    void pageDownloaded(QNetworkReply * pReply);
private:
    QNetworkAccessManager WebControl;
    QByteArray DownloadedData;
};

#endif // PAGEDOWNLOADER_H
