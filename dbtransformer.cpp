#include "dbtransformer.hpp"

DBTransformer::DBTransformer(QObject * parent): QObject(parent) {
    appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    appDataDir = QDir(appDataLocation);
    qDebug() << appDataLocation;
}

int DBTransformer::transform(QString const & fileName) {
    if (!loadInformation(fileName, mat)) return -1;
    if (!translateCategory(mat, categories)) return -1;
    if (!translateMarks(mat, marks)) return -1;

    collection.fill(matrix_t{}, outputFileNumber);

    split(mat, collection, outputFileNumber);

    emit signal_ShowProgress();
    outputSource(collection, outputFileName);
    outputSourceIndex(collection, outputFileName);
    outputSourceReverseIndex(collection, outputFileName);
    emit signal_HideProgress();

    mat.clear();
    for (auto && i : collection) {
        i.clear();
    }
    collection.clear();
    return 0;
}

bool DBTransformer::loadInformation(QString const & fileName, matrix_t & mat) {
    emit updateStatus("Loading database from downloaded file...");
    std::string line;
    if (fileName.isEmpty()) { return false; }
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: can't open " << fileName << Qt::endl;
        return false;
    }
    QTextStream qts(&f);
    while (!qts.atEnd()) {
        auto line = qts.readLine();
        auto entries = line.split(';');
//        qDebug() << entries;
        auto lemma = entries.at(0);
        auto index = entries.at(1);
        auto category = entries.at(2);
        auto unknown = entries.at(3);
        auto form = entries.at(4);
        auto mark = entries.at(5);
        entry_t entry;
        entry[0] = lemma;
        entry[1] = index;
        entry[2] = category;
        entry[3] = unknown;
        entry[4] = form;
        entry[5] = mark;
        mat.push_back(entry);
    }
    return true;
}

bool DBTransformer::translateCategory(matrix_t & mat, rules_t & categories) {
    emit updateStatus("Translating category...");
    QFile file(":/alphabet/rules/cat.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Error: can't open cat.txt" << std::endl;
        return false;
    }

    QTextStream qts(&file);
    while (!qts.atEnd()) {
        QString qline = qts.readLine();
        auto strArray = qline.split('|');
        categories.insert(std::make_pair(strArray.at(0), strArray.at(1)));
    }

    for (auto && i : mat) {
        auto before = i.at(2);
        auto after = categories[before];
        i.at(2) = after;
    }

    file.close();
    return true;
}

bool DBTransformer::translateMarks(matrix_t & mat, rules_t marks) {
    emit updateStatus("Translating inflectional information...");
    QFile file(":/alphabet/rules/rules.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        std::cout << "Error: can't open rules.txt" << std::endl;
        return false;
    }

    QTextStream qts(&file);
    while (!qts.atEnd()) {
        QString qline = qts.readLine();
        if (!qline.isEmpty()) {
            auto strArray = qline.split('|');
            marks.insert(std::make_pair(strArray.at(0), strArray.at(1)));
        }
    }

    int ins = 1;
    for (auto && i : mat) {
        auto before = i.at(5);
        auto after = marks[before];
        i.at(5) = after;
//        qDebug() << i.at(5);
    }
    file.close();
    return true;
}


void DBTransformer::split(matrix_t const & mat, matcol_t & col, int num) {
    emit signal_ShowProgress();
    auto average = mat.size() / num + 1;
//    QString entryToCompare, indexToCompare;

    emit updateStatus("Splitting database...");

    for (auto i = 0; i < 8; ++i) {
        auto && matRef = col[i];
        if (i < 7) {
            matRef = QVector(mat.begin() + i * average, mat.begin() + (1 + i) * average);
        }
        else {
            matRef = QVector(mat.begin() + i * average, mat.end());
        }
        emit signal_UpdateProgress(i, 8);
    }

    emit signal_HideProgress();
}

void DBTransformer::outputSource(matcol_t const & col, QString const & fileName) {
    emit updateStatus("Generaing db1 files...");
    std::cout << "Generaing db1 files..." << std::endl;

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (!hashTable.open(QIODevice::WriteOnly)) { return; }
    QTextStream dbstream(&hashTable);

    auto sourceLocation = appDataLocation + "/db1";
    auto sourceDir = QDir(sourceLocation);
    sourceDir.removeRecursively();
    sourceDir.mkpath(sourceDir.absolutePath());
    QDir::setCurrent(sourceDir.absolutePath());

    for (auto i = 1; i <= col.size(); ++i) {
        auto && matRef = col[i - 1];
        auto currentFileName = fileName + QString(std::to_string(i).c_str());
        qDebug() << currentFileName;
        QFile file(currentFileName);
        QTextStream qts(&file);
        if (file.open(QIODevice::ReadWrite)) {
            QString buffer;
            QTextStream stream(&buffer);
            for (auto j = 0; j < matRef.size(); ++j) {
                auto && entryRef = matRef[j];
                stream << entryRef[0] << ";" << entryRef[1] << ";" << entryRef[2] << ";" << entryRef[4] << ";" << entryRef[5] << Qt::endl;
            }
            qts << buffer;
            emit signal_UpdateProgress(i, 24);
        }
        file.close();
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";";
    }

    hashTable.close();
    std::cout << "db1 files generation complete!" << std::endl;
    emit updateStatus("db1 files generation complete!");
}

void DBTransformer::outputSourceIndex(matcol_t const & col, QString const & fileName) {
    std::cout << "Generating db2 files..." << std::endl;
    emit updateStatus("Generating db2 files...");

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (hashTable.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream dbstream(&hashTable);

    auto SIndexLocation = appDataLocation + "/db2";
    auto SIndexDir = QDir(SIndexLocation);
    SIndexDir.removeRecursively();
    SIndexDir.mkpath(SIndexDir.absolutePath());
    QDir::setCurrent(SIndexDir.absolutePath());

    QString currentEntry;
    for (auto i = 1; i <= col.size(); ++i) {
        auto && matRef = col[i - 1];
        auto currentFileName = fileName + QString::number(i);
        qDebug() << currentFileName;
        QFile file(currentFileName);
        QTextStream qts(&file);
        if (file.open(QIODevice::ReadWrite)) {
            QString buffer;
            QTextStream stream(&buffer);
            for (auto j = 0; j < matRef.size(); ++j) {
                auto && entryRef = matRef[j];
                if (currentEntry != entryRef[0]) {
                    stream << entryRef[0] << ";" << j << Qt::endl;
                    currentEntry = entryRef[0];
                }
            }
            qts << buffer;
            emit signal_UpdateProgress(i + 8, 24);
        }
        file.close();
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";";
    }

    hashTable.close();
    std::cout << "db2 files generation complete!" << std::endl;
    emit updateStatus("db2 files generation complete!");
    }
}

void DBTransformer::outputSourceReverseIndex(matcol_t const & col, QString const & fileName) {
    std::cout << "Generating db3 files..." << std::endl;
    emit updateStatus("Generating db3 files...");

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (!hashTable.open(QIODevice::WriteOnly | QIODevice::Append)) { return; }
    QTextStream dbstream(&hashTable);

    auto SRIndexLocation = appDataLocation + "/db3";
    auto SRIndexDir = QDir(SRIndexLocation);
    SRIndexDir.removeRecursively();
    SRIndexDir.mkpath(SRIndexDir.absolutePath());
    QDir::setCurrent(SRIndexDir.absolutePath());

    for (auto i = 1; i <= col.size(); ++i) {
        auto && matRef = col[i - 1];
        auto currentFileName = fileName + QString::number(i);
        qDebug() << currentFileName;
        QFile file(currentFileName);
        QTextStream qts(&file);
        if (file.open(QIODevice::ReadWrite)) {
            QString buffer;
            QTextStream stream(&buffer);
            for (auto j = 0; j < matRef.size(); ++j) {
                auto && entryRef = matRef[j];
                stream << entryRef[4] << Qt::endl;
            }
            qts << buffer;
        }
        file.close();
        emit signal_UpdateProgress(i + 16, 24);
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";";
    }

    hashTable.close();
    std::cout << "db3 files generation complete!" << std::endl;
    emit updateStatus("db3 files generation complete!");
}

QString DBTransformer::hashFile(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm) {
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result().toHex();
        }
    }
    return QString();
}
