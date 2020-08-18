#include "dbtransformer.hpp"

DBTransformer::DBTransformer(QObject * parent): QObject(parent) {
    appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    appDataDir = QDir(appDataLocation);
    qDebug() << appDataLocation;
}

int DBTransformer::transform(const std::string & fileName) {
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
    return 0;
}

bool DBTransformer::loadInformation(std::string const & fileName, matrix_t & mat) {
    emit updateStatus("Loading database from downloaded file...");
    std::string line;
    if (fileName.empty()) { return false; }
    std::ifstream ifs(fileName);
    while (std::getline(ifs, line)) {
        auto entries = QString(line.c_str()).split(';');
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
    std::string line;
    QFile file(":/alphabet/rules/cat.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        std::cout << "Error: can't open cat.txt" << std::endl;
        return false;
    }
    QTextStream qts(&file);
    while (!qts.atEnd()) {
        QString qline = qts.readLine();
        line = qline.toStdString();
        auto pos = line.find_first_of(' ');
        auto firstHalf = line.substr(0, pos);
        auto secondHalf = line.substr(pos + 1, line.length() - firstHalf.length() - 1);
        categories.insert(std::make_pair(firstHalf.c_str(), secondHalf.c_str()));
    }

    int ins = 1, max = mat.size();
    for (auto && i : mat) {
        auto before = i.at(2);
        auto after = categories[before];
        i.at(2) = after;
    }

    return true;
}

bool DBTransformer::translateMarks(matrix_t & mat, rules_t marks) {
    emit updateStatus("Translating inflectional information...");
    std::string line;
    QFile file(":/alphabet/rules/rules.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        std::cout << "Error: can't open cat.txt" << std::endl;
        return false;
    }
    QTextStream qts(&file);
    while (!qts.atEnd()) {
        QString qline = qts.readLine();
        line = qline.toStdString();
        auto pos = line.find_first_of(' ');
        auto firstHalf = line.substr(0, pos);
        auto secondHalf = line.substr(pos + 1, line.length() - firstHalf.length() - 1);
        marks.insert(std::make_pair(firstHalf.c_str(), secondHalf.c_str()));
    }

    int ins = 1, max = mat.size();
    for (auto && i : mat) {
        auto before = i.at(5);
        auto after = marks[before];
        i.at(5) = after;
    }

    return true;
}


void DBTransformer::split(matrix_t const & mat, matcol_t & col, int num) {
    emit signal_ShowProgress();
    auto average = mat.size() / num + 1;
    QString entryToCompare, catToCompare;
    std::ostringstream oss;
    int fileIndex = 0;
    int entryIndex = 0;
    emit updateStatus("Splitting database...");

    for (auto && i : mat) {
        if (entryIndex < average) {
            entryToCompare = i.at(0);
            catToCompare = i.at(2);
            auto && matRef = col[fileIndex];
            auto entry = entry_t {};
            for (auto j = 0; j < i.size(); ++j) {
                entry[j] = i[j];
            }
            matRef.push_back(entry);
            ++entryIndex;
        }
        else {
            auto currentEntry = i.at(0);
            auto currentCat = i.at(2);
            if (currentEntry == entryToCompare && currentCat == catToCompare) {
                auto && matRef = col[fileIndex];
                auto entry = entry_t{};
                for (auto j = 0; j < i.size(); ++j) {
                    entry[j] = i[j];
                }
                matRef.push_back(entry);
                ++entryIndex;
            }
            else {
                entryIndex = 0;
                ++fileIndex;
                entryToCompare = i.at(0);
                catToCompare = i.at(2);
                auto && matRef = col[fileIndex];
                auto entry = entry_t{};
                for (auto j = 0; j < i.size(); ++j) {
                    entry[j] = i[j];
                }
                matRef.push_back(entry);
                ++entryIndex;
            }
        }
        emit signal_UpdateProgress(fileIndex, 8);
    }
    emit signal_HideProgress();
}

void DBTransformer::outputSource(matcol_t const & col, std::string const & fileName) {
    emit updateStatus("Generaing db1 files...");
    std::cout << "Generaing db1 files..." << std::endl;

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (!hashTable.open(QIODevice::WriteOnly)) { return; }
    QTextStream dbstream(&hashTable);

    auto sourceLocation = appDataLocation + "/db1";
    auto sourceDir = QDir(sourceLocation);
    sourceDir.mkpath(sourceDir.absolutePath());
    QDir::setCurrent(sourceDir.absolutePath());

    QString qfileName = fileName.c_str();

    for (auto i = 1; i <= col.size(); ++i) {
        auto && matRef = col[i - 1];
        auto currentFileName = qfileName + QString(std::to_string(i).c_str());
        qDebug() << currentFileName;
        QFile file(currentFileName);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            std::ostringstream oss;
            QString string;
            for (auto j = 0; j < matRef.size(); ++j) {
                auto && entryRef = matRef[j];
                oss << entryRef[0].toStdString() << "; " << entryRef[1].toStdString() << "; " << entryRef[2].toStdString() << "; " << entryRef[4].toStdString() << "; " << entryRef[5].toStdString() << std::endl;
            }
            stream << QString::fromStdString(oss.str());
            emit signal_UpdateProgress(i, 24);
        }
        file.close();
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";;;";
    }

    hashTable.close();
    std::cout << "db1 files generation complete!" << std::endl;
    emit updateStatus("db1 files generation complete!");
}

void DBTransformer::outputSourceIndex(matcol_t const & col, std::string const & fileName) {
    std::cout << "Generating db2 files..." << std::endl;
    emit updateStatus("Generating db2 files...");

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (hashTable.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream dbstream(&hashTable);

    auto SIndexLocation = appDataLocation + "/db2";
    auto SIndexDir = QDir(SIndexLocation);
    SIndexDir.mkpath(SIndexDir.absolutePath());
    QDir::setCurrent(SIndexDir.absolutePath());

    QString currentEntry;
    for (auto i = 1; i <= col.size(); ++i) {
        auto && matRef = col[i - 1];
        auto currentFileName = QString(fileName.c_str()) + QString(std::to_string(i).c_str());
        qDebug() << currentFileName;
        QFile file(currentFileName);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream stream(&file);
            std::ostringstream oss;
            for (auto j = 0; j < matRef.size(); ++j) {
                auto && entryRef = matRef[j];
                if (currentEntry != entryRef[0]) {
                    oss << entryRef[0].toStdString() << "; " << j << std::endl;
                    currentEntry = entryRef[0];
                }
            }
            stream << QString::fromStdString(oss.str());
            emit signal_UpdateProgress(i + 8, 24);
        }
        file.close();
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";;;";
    }

    hashTable.close();
    std::cout << "db2 files generation complete!" << std::endl;
    emit updateStatus("db2 files generation complete!");
    }
}

void DBTransformer::outputSourceReverseIndex(matcol_t const & col, std::string const & fileName) {
    std::cout << "Generating db3 files..." << std::endl;
    emit updateStatus("Generating db3 files...");

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (!hashTable.open(QIODevice::WriteOnly | QIODevice::Append)) { return; }
    QTextStream dbstream(&hashTable);

    auto SRIndexLocation = appDataLocation + "/db3";
    auto SRIndexDir = QDir(SRIndexLocation);
    SRIndexDir.mkpath(SRIndexDir.absolutePath());
    QDir::setCurrent(SRIndexDir.absolutePath());

    for (auto i = 1; i <= col.size(); ++i) {
        auto && matRef = col[i - 1];
        auto currentFileName = QString(fileName.c_str()) + QString(std::to_string(i).c_str());
        qDebug() << currentFileName;
        QFile file(currentFileName);
        if (file.open(QIODevice::ReadWrite)) {
            std::ostringstream oss;
            for (auto j = 0; j < matRef.size(); ++j) {
                auto && entryRef = matRef[j];
                oss << entryRef[4].toStdString() << std::endl;
            }
            auto && str = oss.str();
            QByteArray byteData(str.c_str());
            file.write(byteData);
        }
        file.close();
        emit signal_UpdateProgress(i + 16, 24);
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";;;";
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
