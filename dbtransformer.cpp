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
    outputSource(collection, outputFileName);
    outputSourceIndex(collection, outputFileName);
    outputSourceReverseIndex(collection, outputFileName);
    return 0;
}

bool DBTransformer::loadInformation(std::string const & fileName, matrix_t & mat) {
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

    for (auto && i : mat) {
        auto before = i.at(2);
        auto after = categories[before];
        i.at(2) = after;
    }
    return true;
}

bool DBTransformer::translateMarks(matrix_t & mat, rules_t marks) {
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

    for (auto && i : mat) {
        auto before = i.at(5);
        auto after = marks[before];
        i.at(5) = after;
    }

    return true;
}


void DBTransformer::split(matrix_t const & mat, matcol_t & col, int num) {
    auto average = mat.size() / num + 1;
    QString entryToCompare, catToCompare;
    std::ostringstream oss;
    int fileIndex = 0;
    int entryIndex = 0;

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
    }
}

void DBTransformer::outputSource(matcol_t const & col, std::string const & fileName) {
    std::cout << "Generaing source files..." << std::endl;

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (!hashTable.open(QIODevice::WriteOnly)) { return; }
    QTextStream dbstream(&hashTable);

    auto sourceLocation = appDataLocation + "/source";
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
        }
        file.close();
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";;;";
    }

    hashTable.close();
    std::cout << "source files generation complete!" << std::endl;
}

void DBTransformer::outputSourceIndex(matcol_t const & col, std::string const & fileName) {
    std::cout << "Generating source_index files..." << std::endl;

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (hashTable.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream dbstream(&hashTable);

    auto SIndexLocation = appDataLocation + "/source_index";
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
        }
        file.close();
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";;;";
    }

    hashTable.close();
    std::cout << "source_index files generation complete!" << std::endl;
    }
}

void DBTransformer::outputSourceReverseIndex(matcol_t const & col, std::string const & fileName) {
    std::cout << "Generating source_reverse_index files..." << std::endl;

    QFile hashTable(appDataLocation + "/.DBHashes");
    if (!hashTable.open(QIODevice::WriteOnly | QIODevice::Append)) { return; }
    QTextStream dbstream(&hashTable);

    auto SRIndexLocation = appDataLocation + "/source_reverse_index";
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
        dbstream << hashFile(currentFileName, QCryptographicHash::Md5) << ";;;";
    }

    hashTable.close();
    std::cout << "source_reverse_index files generation complete!" << std::endl;
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
