#ifndef DBUPDATEDIALOG_HPP
#define DBUPDATEDIALOG_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QTimer>
#include <QThread>

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

private slots:
    void acceptUpdate(QString const);
    void process();
    void acceptCleanedUp();

private:
    QString appDataLocation;
    DBDownloaderHelper * m_DBDownloadHelper;
    DBDownloader * m_DBDownloader;

    int examineBINDBs();
    QString hashFile(QString const &, QCryptographicHash::Algorithm = QCryptographicHash::Md5);

};


#endif // DBUPDATEDIALOG_HPP
