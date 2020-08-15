#ifndef PAGEDOWNLOADER_H
#define PAGEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QEventLoop>
#include <QTimer>

class PageDownloader : public QObject
{
    Q_OBJECT
public:
    PageDownloader(QNetworkAccessManager *, QObject * parent = nullptr);
    virtual ~PageDownloader();
    void DownloadPage(QUrl pageUrl);
    QByteArray downloadedData() const;

signals:
    void downloaded();
    void connectionError();
    void timeoutError();

private slots:
    void pageDownloaded(QNetworkReply * pReply);
private:
    QNetworkAccessManager * WebControl;
    QByteArray DownloadedData;
};

#endif // PAGEDOWNLOADER_H
