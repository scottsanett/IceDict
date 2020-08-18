#ifndef DBUPDATEDIALOG_HPP
#define DBUPDATEDIALOG_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QTimer>
#include <QThread>
#include <QProgressBar>

#include "dbdownloader.hpp"
#include "dbtransformer.hpp"

namespace Ui {
    class DBUpdateDialog;
}


class DBUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DBUpdateDialog(QWidget *parent = nullptr);
    ~DBUpdateDialog();
    void assignDBDownloader(DBDownloader*, DBDownloaderHelper*);

    /*
    void setRange(int minimum, int maximum);
    void setValue(int value);
    void setLabelText(QString labtext);
    bool wasCanceled();
    */

signals:

private:
    Ui::DBUpdateDialog *ui;
    bool cancelStatus;
    QString appDataLocation;
    QTextBrowser * textBrowser;
    QProgressBar * pBar;
    DBDownloader * m_DBDownloader;
    DBDownloaderHelper * m_DBDownloadHelper;

    const char * BINDBUrl = "https://bin.arnastofnun.is/django/api/nidurhal/?file=SHsnid.csv.zip";

    /*
    int examineBINDBs();
    QString hashFile(QString const &, QCryptographicHash::Algorithm = QCryptographicHash::Md5) {}
    */

private slots:
    void cancel() {}
    void displayMsg(QString const msg);
    void appendMsg(QString const msg);
    void closeDialog();
    void slot_ShowDownloadProgress();
    void slot_UpdateDownloadProgress(qint64, qint64);
    void slot_HideDownloadProgress();

    void slot_ShowTransformProgress();
    void slot_UpdateTransformProgress(qint64, qint64);
    void slot_HideTransformProgress();
};

class DBUpdateDialogThread : public QObject {
    Q_OBJECT

public:
    /*
    explicit DBUpdateDialogThread(QObject * parent = nullptr): QObject(parent) {}
    ~DBUpdateDialogThread() {}
    */

signals:
    void updateStatus(QString const);
    void BINReady();
    void finished();
    void error(QString err);

    void signal_ShowDownloadProgress();
    void signal_UpdateDownloadProgress(qint64, qint64);
    void signal_HideDownloadProgress();

    void signal_ShowTransformProgress();
    void signal_UpdateTransformProgress(qint64, qint64);
    void signal_HideTransformProgress();

private slots:
    void acceptUpdate(QString const);
    void process();
    void acceptCleanedUp();
    void slot_ShowDownloadProgress();
    void slot_UpdateDownloadProgress(qint64, qint64);
    void slot_HideDownloadProgress();

    void slot_ShowTransformProgress();
    void slot_UpdateTransformProgress(qint64, qint64);
    void slot_HideTransformProgress();

private:
    QString appDataLocation;
    DBDownloaderHelper * m_DBDownloadHelper;
    DBDownloader * m_DBDownloader;

    int examineBINDBs();
    QString hashFile(QString const &, QCryptographicHash::Algorithm = QCryptographicHash::Md5);

};


#endif // DBUPDATEDIALOG_HPP
