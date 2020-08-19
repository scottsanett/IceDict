#ifndef DBTRANSFORMER_HPP
#define DBTRANSFORMER_HPP

#include <QString>
#include <QVector>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>
#include <QStandardPaths>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include <map>
#include <utility>
#include <QCryptographicHash>
#include <QDataStream>

using entry_t = std::array<QString, 6>;
using matrix_t = QVector<entry_t>;
using matcol_t = QVector<matrix_t>;
using rules_t = std::map<QString, QString>;


class DBTransformer: public QObject {
    Q_OBJECT
public:
    explicit DBTransformer(QObject * parent = nullptr);
    virtual ~DBTransformer() {}
    int transform(QString const &);

signals:
    void updateStatus(QString const);
    void signal_ShowProgress();
    void signal_HideProgress();
    void signal_UpdateProgress(qint64, qint64);

private:
    const char * outputFileName = "part";
    const int outputFileNumber = 8;
    QString appDataLocation;
    QDir appDataDir;

    QString hashTableContent;

    matrix_t mat;
    rules_t categories, marks;
    QString fileName, nameOfOutput;
    int numOfFiles;
    matcol_t collection;

    bool loadInformation(QString const & fileName, matrix_t & mat);
    bool translateCategory(matrix_t & mat, rules_t & categories);
    bool translateMarks(matrix_t & mat, rules_t marks);
    void split(matrix_t const & mat, matcol_t & col, int num);
    void outputSource(matcol_t const & col, QString const & fileName);
    void outputSourceIndex(matcol_t const & col, QString const & fileName);
    void outputSourceReverseIndex(matcol_t const & col, QString const & fileName);
    QString hashFile(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);

};

#endif // DBTRANSFORMER_HPP
