#include "dbupdatedialog.hpp"
#include "ui_dbupdatedialog.h"

DBUpdateDialog::DBUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DBUpdateDialog)
{
    appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    Q_INIT_RESOURCE(resource);
    ui->setupUi(this);
    this->setWindowTitle("BÍN Database Updater");
    this->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    this->setModal(true);
    auto layout = new QVBoxLayout(this);
    this->setLayout(layout);
    textBrowser = new QTextBrowser(this);
//    auto buttonBox = ui->buttonBox;
    layout->addWidget(textBrowser);
    textBrowser->setText("Checking BÍN database integrity...");
//    layout->addWidget(buttonBox);
    show();

    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(screenGeometry.center() - this->rect().center());
}

DBUpdateDialog::~DBUpdateDialog()
{
    delete ui;
}

void DBUpdateDialog::displayMsg(QString const msg) {
    textBrowser->setText(msg);
}

void DBUpdateDialog::appendMsg(QString const msg) {
    auto text = textBrowser->toPlainText();
    text += "\n" + msg;
    textBrowser->setText(text);
}

void DBUpdateDialog::closeDialog() {
    this->close();
}

void DBUpdateDialog::slot_ShowProgress() {
    if (pBar) pBar->deleteLater();
    pBar = new QProgressBar(this);
    this->layout()->addWidget(pBar);
}

void DBUpdateDialog::slot_UpdateProgress(qint64 ist, qint64 max) {
    pBar->setRange(0, max);
    pBar->setValue(ist);
}

void DBUpdateDialog::slot_HideProgress() {
    pBar->close();
    pBar->deleteLater();
    pBar = nullptr;
}

void DBUpdateDialogThread::acceptUpdate(QString const str) {
    emit updateStatus(str);
}

void DBUpdateDialogThread::process() {
    appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    bool BINDBStatus = false;

    if (m_CheckIntegrity) {
        BINDBStatus = examineBINDBs();
    }

    if (!m_CheckIntegrity || BINDBStatus != 0) {

        m_DBDownloadHelper = new DBDownloaderHelper(this);
        m_DBDownloader = new DBDownloader(m_DBDownloadHelper, this);

        connect (m_DBDownloadHelper, SIGNAL(downloaded(int)),
                 m_DBDownloader, SLOT(processFile(int)));
        connect (m_DBDownloadHelper, SIGNAL(updateStatus(QString const)),
                 this, SLOT(acceptUpdate(QString const)));
        connect (m_DBDownloadHelper, SIGNAL(signal_ShowProgress()),
                 this, SLOT(slot_ShowProgress()));
        connect (m_DBDownloadHelper, SIGNAL(signal_UpdateProgress(qint64, qint64)),
                 this, SLOT(slot_UpdateProgress(qint64, qint64)));
        connect (m_DBDownloadHelper, SIGNAL(signal_HideProgress()),
                 this, SLOT(slot_HideProgress()));

        connect (m_DBDownloader, SIGNAL(updateStatus(QString const)),
                 this, SLOT(acceptUpdate(QString const)));
        connect (m_DBDownloader, SIGNAL(cleanedUp()),
                 this, SLOT(acceptCleanedUp()));
        connect (m_DBDownloader, SIGNAL(signal_ShowProgress()),
                 this, SLOT(slot_ShowProgress()));
        connect (m_DBDownloader, SIGNAL(signal_UpdateProgress(qint64, qint64)),
                 this, SLOT(slot_UpdateProgress(qint64, qint64)));
        connect (m_DBDownloader, SIGNAL(signal_HideProgress()),
                 this, SLOT(slot_HideProgress()));

        m_DBDownloadHelper->proceed();
    }
    else {
        emit acceptCleanedUp();
    }
}

void DBUpdateDialogThread::slot_ShowProgress() {
    emit signal_ShowProgress();
}


void DBUpdateDialogThread::slot_UpdateProgress(qint64 a, qint64 b) {
    emit signal_UpdateProgress(a, b);
}

void DBUpdateDialogThread::slot_HideProgress() {
    emit signal_HideProgress();
}

void DBUpdateDialogThread::acceptCleanedUp() {
    emit finished();
}

void DBUpdateDialogThread::checkIntegrity(bool f) {
    m_CheckIntegrity = f;
}


int DBUpdateDialogThread::examineBINDBs() {
    emit signal_ShowProgress();
    QDir::setCurrent(appDataLocation);
    qDebug() << appDataLocation;
    if (!QFileInfo(".DBHashes").exists()) {
        qDebug() << "Database hash list does not exist; creating one...";
        emit updateStatus("Database hash list does not exist; creating one...");
        emit signal_HideProgress();
        return -1;
    }
    else {
        QFile f(".DBHashes");
#ifdef _WIN32
        auto fullPath = appDataLocation + "/.DBHashes";
        SetFileAttributes(fullPath.toStdWString().c_str(), FILE_ATTRIBUTE_HIDDEN);
#endif
        if (!f.open(QIODevice::ReadOnly)) {
            qDebug() << "Cannot open hash list";
            emit updateStatus("Cannot open hash list");
            return -1;
        }
        QTextStream qts(&f);
        auto hashList = qts.readAll().split(";");
        f.close();

        if (hashList.length() != 25) return -1;

        for (auto i = 0; i < 8; ++i) {
            auto fileName = appDataLocation + "/db1/part" + QString::number(i + 1);
            auto currentHash = hashFile(fileName);
            if (hashList.at(i) != currentHash) {
                qDebug() << hashList.at(i) << currentHash;
                qDebug() << "Database found but deprecated. Rebuilding dabatase...";
                emit updateStatus("Database found but deprecated. Rebuilding dabatase...");
                return -1;
            }
            emit signal_UpdateProgress(i + 1, 24);
        }

        for (auto i = 0; i < 8; ++i) {
            auto fileName = appDataLocation + "/db2/part" + QString::number(i + 1);
            auto currentHash = hashFile(fileName);
            if (hashList.at(i + 8) != currentHash) {
                qDebug() << hashList.at(i + 8) << currentHash;
                qDebug() << "Database found but deprecated. Rebuilding dabatase...";
                emit updateStatus("Database found but deprecated. Rebuilding dabatase...");
                return -1;
            }
            emit signal_UpdateProgress(i + 9, 24);
        }

        for (auto i = 0; i < 8; ++i) {
            auto fileName = appDataLocation + "/db3/part" + QString::number(i + 1);
            if (hashList.at(i + 16) != hashFile(fileName)) {
                qDebug() << hashList.at(i) << hashFile(fileName);
                qDebug() << "Database found but deprecated. Rebuilding dabatase...";
                emit updateStatus("Database found but deprecated. Rebuilding dabatase...");
                return -1;
            }
            emit signal_UpdateProgress(i + 17, 24);
        }
        emit signal_HideProgress();
        return 0;
    }
}

QString DBUpdateDialogThread::hashFile(QString const & fileName, QCryptographicHash::Algorithm hashAlgorithm) {
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result().toHex();
        }
    }
    return QString();
}
