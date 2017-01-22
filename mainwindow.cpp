#include "QObject"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    Q_INIT_RESOURCE(resource);
    inflectionals = mapptrvecptr_t(new mapptrvec_t);
    definitions = mapptrvecptr_t(new mapptrvec_t);
    originals = mapptrvecptr_t(new mapptrvec_t);
    wordindex = strsetptr_t(new strset_t);
    dictionaries = strvecptrmapptrvecptr_t(new strvecptrmapptrvec_t);
    forms = strsetptr_t(new strset_t);
    allInflectionalWords = strsetptr_t(new strset_t);
    for (size_t i = 0; i < 7; ++i) {
        inflectionals->push_back(mapptr_t(new map_t));
        originals->push_back(mapptr_t(new map_t));
    }
    for (size_t i = 0; i < 2; ++i) {
        definitions->push_back(mapptr_t(new map_t));
        dictionaries->push_back(strvecptrmapptr_t(new strvecptrmap_t));
    }
    std::thread t_wordindex(&MainWindow::importWordIndex, this);
    t_wordindex.join();
    std::string zname = "zoega";
    std::string vname = "vifgusson";
    std::thread t_zoega(&MainWindow::importWordIndexThread, this, definitions, zname, 0);
    std::thread t_vifgusson(&MainWindow::importWordIndexThread, this, definitions, vname, 1);
    t_zoega.join(); t_vifgusson.join();
    ui->setupUi(this);
    this->setWindowTitle("Old Icelandic Dictionary");
    QIcon icon(":/alphabet/icon.ico");
    this->setWindowIcon(icon);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buttonChangeColor() {
    ui->searchIcl->setStyleSheet("");
    ui->searchTxt->setStyleSheet("");
//    ui->searchReg->setStyleSheet("");
    ui->searchOrig->setStyleSheet("");
    ui->plot->setStyleSheet("");
    ui->printOne->setStyleSheet("");
    if (flags[0] == 1) { ui->searchIcl->setStyleSheet("background-color: grey"); }
    else if (flags[1] == 1) { ui->searchTxt->setStyleSheet("background-color: grey"); }
//    else if (flags[2] == 1) { ui->searchReg->setStyleSheet("background-color: grey"); }
    else if (flags[3] == 1) { ui->searchOrig->setStyleSheet("background-color: grey"); }
    else if (flags[4] == 1) { ui->plot->setStyleSheet("background-color: grey"); }
    else if (flags[5] == 1) { ui->printOne->setStyleSheet("background-color: grey"); }
    else{}
}

void MainWindow::addStyleToResults(std::string & line) {
    std::istringstream iss(line);
    std::string key;
    iss >> key;
    if (key.back() != ';') {
        std::string temp;
        iss >> temp;
        key += ' ' + temp;
    }
    else {
        key = key.substr(0, key.length() - 1);
    }
    key = "<th>" + key + "</th>";
    std::string arg2;
    iss >> arg2;
    arg2 = arg2.substr(0, arg2.length() - 1);
    arg2 = "<td>" + arg2 + "</td>";
    std::string arg3;
    iss >> arg3;
    arg3 = arg3.substr(0, arg3.length() - 1);
    arg3 = "<td><font color=\"red\">" + arg3 + "</font></td>";
    std::string arg4;
    std::string temp;
    while (iss >> temp) {
        arg4 += temp + ' ';
    }
    arg4 = arg4.substr(0, arg4.length() - 1);
    arg4 = "<td>" + arg4 + "</td>";
    line = "<tr>" + key + arg2 + arg3 + arg4 + "</tr>";
}

void MainWindow::on_searchIcl_clicked()
{
    flags = {1, 0, 0, 0, 0, 0};
    buttonChangeColor();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
    ui->results->clear();
    ui->options->clear();
//    size_t num = 0;
//    ui->options->addItems(dictentries);
}

void MainWindow::on_searchTxt_clicked()
{
    if (!inflectionReady) {
        Dialog dlg;
        dlg.setModal(true);
        if (!dlg.exec()) { return; }
        importInflections();
    }
    flags = {0, 1, 0, 0, 0, 0};
    buttonChangeColor();
    ui->input->setPlaceholderText("Insert text here...");
    ui->input->clear();
    ui->results->clear();
    ui->options->clear();
}

/*
void MainWindow::on_searchReg_clicked()
{
    flags = {0, 0, 1, 0, 0, 0};
    buttonChangeColor();
    ui->input->setPlaceholderText("Insert regex here...");
    ui->input->clear();
    ui->results->clear();
    ui->options->clear();
}
*/

void MainWindow::on_searchOrig_clicked()
{
    if (!inflectionReady) {
        Dialog dlg;
        dlg.setModal(true);
        if (!dlg.exec()) { return; }
        importInflections();
    }
    flags = {0, 0, 0, 1, 0, 0};
    buttonChangeColor();
    ui->results->clear();
    ui->options->clear();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
}

void MainWindow::on_plot_clicked()
{
    if (!inflectionReady) {
        Dialog dlg;
        dlg.setModal(true);
        if (!dlg.exec()) { return; }
        importInflections();
        ui->results->setText("The dictionary is now ready.");
    }
    flags = {0, 0, 0, 0, 1, 0};
    buttonChangeColor();
    ui->results->clear();
    ui->options->clear();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
}


void MainWindow::on_printOne_clicked()
{
    if (!inflectionReady) {
        Dialog dlg;
        dlg.setModal(true);
        if (!dlg.exec()) { return; }
        importInflections();
        ui->results->setText("The dictionary is now ready.");
    }
    typeTimes = 0;
    flags = {0, 0, 0, 0, 0, 1};
    buttonChangeColor();
    stored.clear();
    ui->results->clear();
    ui->options->clear();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
}

void MainWindow::on_input_textEdited(const QString &arg1)
{
    std::string word = arg1.toStdString();
    ui->options->clear();
    if (flags[0] == 1) // search icelandic on
    {
        if (word.length() > 1) {
            QStringList display;
            for (auto && entry : *wordindex) {
                auto pos = entry.find(word);
                if (pos == 0) {
                    display.push_back(entry.c_str());
                }
            }
            ui->options->addItems(display);
        }
    }
    else if (flags[3] == 1 && inflectionReady) //query on
    {
        if (word.length() > 2) {
            auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
            QStringList display;

            ui->options->addItems(display);
        }
    }
    else if (flags[5] == 1 && inflectionReady) {
        if (typeTimes == 0) { printOneWord = word; }
        else {
            stored.clear();
            printOneForm = word;
            QStringList display;
            std::istringstream iss(word);
            std::string snippet;
            while (iss >> snippet) {
                stored.push_back(snippet);
            }
            for (auto && entry : *forms) {
                auto flag = [&]() { for (auto && snippet : stored) { if (entry.find(snippet) == std::string::npos) return false; } return true; }();
                if (flag == true) {
                    display.push_back(entry.c_str());
                }
            }
            ui->options->addItems(display);
        }
    }
}

void MainWindow::allFormsAutocompleteThread(ptrvecstrvecptr_t results, std::string word, size_t index) {
    auto thisResult = results->at(index);
}

/*  Transfer a string to lower case   */
void MainWindow::toLower(std::string & str) {
    const std::vector<std::string> upper = { "Þ", "Ð", "Æ", "Ö", "Á", "É", "Í", "Ó", "Ú", "Ý", "Œ" };
    const std::vector<std::string> lower = { "þ", "ð", "æ", "ö", "á", "é", "í", "ó", "ú", "ý", "œ" };
    auto f = [&](std::string & name) { for (auto i = 0; i < name.length(); ++i) {
        if (isalpha(name[i])) {
            if (isupper(name[i])) {
                name[i] = tolower(name[i]);
            }
        }
        else {
            if ((name[i] == upper[0][0]) && (name[i + 1] == upper[0][1])) { name[i] = lower[0][0]; name[i + 1] = lower[0][1]; i += 1; }
            else if ((name[i] == upper[1][0]) && (name[i + 1] == upper[1][1])) { name[i] = lower[1][0]; name[i + 1] = lower[1][1]; i += 1; }
            else if ((name[i] == upper[2][0]) && (name[i + 1] == upper[2][1])) { name[i] = lower[2][0]; name[i + 1] = lower[2][1]; i += 1; }
            else if ((name[i] == upper[3][0]) && (name[i + 1] == upper[3][1])) { name[i] = lower[3][0]; name[i + 1] = lower[3][1]; i += 1; }
            else if ((name[i] == upper[4][0]) && (name[i + 1] == upper[4][1])) { name[i] = lower[4][0]; name[i + 1] = lower[4][1]; i += 1; }
            else if ((name[i] == upper[5][0]) && (name[i + 1] == upper[5][1])) { name[i] = lower[5][0]; name[i + 1] = lower[5][1]; i += 1; }
            else if ((name[i] == upper[6][0]) && (name[i + 1] == upper[6][1])) { name[i] = lower[6][0]; name[i + 1] = lower[6][1]; i += 1; }
            else if ((name[i] == upper[7][0]) && (name[i + 1] == upper[7][1])) { name[i] = lower[7][0]; name[i + 1] = lower[7][1]; i += 1; }
            else if ((name[i] == upper[8][0]) && (name[i + 1] == upper[8][1])) { name[i] = lower[8][0]; name[i + 1] = lower[8][1]; i += 1; }
            else if ((name[i] == upper[9][0]) && (name[i + 1] == upper[9][1])) { name[i] = lower[9][0]; name[i + 1] = lower[9][1]; i += 1; }
            else if ((name[i] == upper[10][0]) && (name[i + 1] == upper[10][1])) { name[i] = lower[10][0]; name[i + 1] = lower[10][1]; i += 1; }
            else {}
        }
    } };
    f(str);
}

void MainWindow::importForms() {
    QFile f(":/alphabet/forms");
    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    while (std::getline(file, line)) {
        forms->insert(line);
    }
    f.close();
}

/* import the index for all words in the two dictionaries */
void MainWindow::importWordIndex() {
    QFile wd(":/alphabet/wordindex");
    wd.open(QIODevice::ReadOnly);
    QString file = wd.readAll();
    std::istringstream wordindexfile(file.toStdString());
    std::string line;
    while (std::getline(wordindexfile, line)) {
        wordindex->insert(line);
        dictentries.push_back(line.c_str());
    }
    wd.close();
}

/*import all the inflection forms and its position*/
void MainWindow::importInflectionsThread(mapptrvecptr_t mapvec, size_t i) {
    std::string filename = ":/alphabet/source_reverse_index/part" + std::to_string(i);
    QFile f(filename.c_str());
    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    size_t index = 0;
    auto thisMap = mapvec->at(i - 1);
    while (std::getline(file, line)) {
        thisMap->insert(std::make_pair(line, index));
        ++index;
    }
    f.close();
}


void MainWindow::importInflections() {
    importForms();
    importOriginal();
    importDictionary();
    std::thread t1(&MainWindow::importInflectionsThread, this, inflectionals, 1);
    std::thread t2(&MainWindow::importInflectionsThread, this, inflectionals, 2);
    std::thread t3(&MainWindow::importInflectionsThread, this, inflectionals, 3);
    std::thread t4(&MainWindow::importInflectionsThread, this, inflectionals, 4);
    t1.join(); t2.join(); t3.join(); t4.join();
    std::thread t5(&MainWindow::importInflectionsThread, this, inflectionals, 5);
    std::thread t6(&MainWindow::importInflectionsThread, this, inflectionals, 6);
    std::thread t7(&MainWindow::importInflectionsThread, this, inflectionals, 7);
    t5.join(); t6.join(); t7.join();
    inflectionReady = true;
}


void MainWindow::importOriginalThread(mapptrvecptr_t mapvec, size_t i) {
    std::string filename = ":/alphabet/sources_index/part" + std::to_string(i);
    QFile f(filename.c_str());
    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    auto thisMap = mapvec->at(i - 1);
    while (std::getline(file, line)) {
        std::string key;
        std::istringstream iss(line);
        iss >> key;
        if (key.back() != ';') {
            std::string temp;
            iss >> temp;
            key += " " + temp;
        }
        std::string index;
        iss >> index;
        thisMap->insert(std::make_pair(key, std::stoul(index)));
    }
    f.close();
}

void MainWindow::importOriginal() {
    std::thread t1(&MainWindow::importOriginalThread, this, originals, 1);
    std::thread t2(&MainWindow::importOriginalThread, this, originals, 2);
    std::thread t3(&MainWindow::importOriginalThread, this, originals, 3);
    std::thread t4(&MainWindow::importOriginalThread, this, originals, 4);
    std::thread t5(&MainWindow::importOriginalThread, this, originals, 5);
    std::thread t6(&MainWindow::importOriginalThread, this, originals, 6);
    std::thread t7(&MainWindow::importOriginalThread, this, originals, 7);
    t1.join(); t2.join(); t3.join();
    t4.join(); t5.join(); t6.join(); t7.join();
}


/* import all the inflection words  (without the size_t index)*/

void MainWindow::importWordIndexThread(mapptrvecptr_t mapvec, std::string const name, size_t i) {
    std::string filename = ":/alphabet/" + name;
    QFile f(filename.c_str());
    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    auto thisMap = mapvec->at(i);
    size_t index = 0;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        thisMap->insert(std::make_pair(key, index));
        ++index;
    }
    f.close();
}

void MainWindow::importDictionary() {
    std::thread t0(&MainWindow::importDictionaryThread, this, "zoega", 0);
    std::thread t1(&MainWindow::importDictionaryThread, this, "vifgusson", 1);
    t0.join(); t1.join();
}

void MainWindow::importDictionaryThread(std::string const name, size_t i) {
    std::string filename = ":/alphabet/" + name;
    QFile f(filename.c_str());
    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    auto thisMap = dictionaries->at(i);
    while (std::getline(file, line)) {
        auto entry = strvecptr_t(new strvec_t);
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        std::string sense;
        std::string temp;
        while (iss >> temp) {
            sense += ' ' + temp;
            if (temp.back() == ';') {
               sense += '\n';
               entry->push_back(sense);
               sense = "";
            }
        }
        thisMap->insert(std::make_pair(key, entry));
    }
    f.close();
}

void MainWindow::findDefinition(std::string const & words) {
    std::vector<std::string> results;
    std::string word = "<b>" + words + "</b>";
    QFile zfile(":/alphabet/zoega");
    QFile vfile(":/alphabet/vifgusson");
    zfile.open(QIODevice::ReadOnly);
    vfile.open(QIODevice::ReadOnly);
    QString ztext = zfile.readAll();
    QString vtext = vfile.readAll();
    std::istringstream ziss(ztext.toStdString());
    std::istringstream viss(vtext.toStdString());
    auto zitr = definitions->at(0)->find(word);
    auto vitr = definitions->at(1)->find(word);
    std::string line;
    if (zitr != definitions->at(0)->end()) {
        size_t zpos = zitr->second;
//        qDebug() << zpos << '\n';
        auto i = 0;
        while (std::getline(ziss, line)) {
            if (i == zpos) {
                results.push_back(line);
//                qDebug() << line.c_str() << '\n';
                break;
            }
            ++i;
        }
    }
    if (vitr != definitions->at(1)->end()) {
        auto vpos = vitr->second;
        auto i = 0;
        while (std::getline(viss, line)) {
            if (i == vpos) {
                results.push_back(line + '\n');
                break;
            }
            ++i;
        }
    }
    std::string display;
    for (auto && i : results) {
        display += i + "\n\n";
    }
    display = "<p align=\"justify\">" + display + "</p>";
    ui->results->setHtml(display.c_str());
    zfile.close();
    vfile.close();
}

void MainWindow::findInflection(std::string const & word) {
    auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
    for (auto i = 0; i < 7; ++i) {
        results->push_back(strvecptr_t(new strvec_t));
    }
    std::thread t0(&MainWindow::findInflectionThread, this, results, word, 0);
    std::thread t1(&MainWindow::findInflectionThread, this, results, word, 1);
    std::thread t2(&MainWindow::findInflectionThread, this, results, word, 2);
    std::thread t3(&MainWindow::findInflectionThread, this, results, word, 3);
    std::thread t4(&MainWindow::findInflectionThread, this, results, word, 4);
    std::thread t5(&MainWindow::findInflectionThread, this, results, word, 5);
    std::thread t6(&MainWindow::findInflectionThread, this, results, word, 6);
    t0.join(); t1.join(); t2.join(); t3.join(); t4.join(); t5.join(); t6.join();
    auto resultSize = [&]() { size_t sz = 0; for (auto i : *results) { sz += i->size(); } return sz; }();
    if (resultSize == 0) {
        ui->results->setText("Word not found.");
        return;
    }
    std::string toprint;
    for (auto && i : *results) {
        for (auto && j : *i) {
            addStyleToResults(j);
            toprint += j + "\n\n";
        }
    }
    toprint = "<p align=\"center\"><table border=\"1\" cellpadding=\"20\">" + toprint + "</table></p>";
    ui->results->setHtml(toprint.c_str());
}

void MainWindow::findInflectionThread(ptrvecstrvecptr_t results, std::string word, size_t index) {
    auto thisDic = inflectionals->at(index);
    auto thisResult = results->at(index);
    auto itr = thisDic->find(word);
    if (itr == thisDic->end()) { return; }
    std::string filename = ":/alphabet/sources/part" + std::to_string(index + 1);
    QFile file(filename.c_str());
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line;
    auto key = itr->first;
    auto pos = itr->second;
    auto currentPos = 0;
    while (std::getline(issfile, line)) {
        if (currentPos != pos) {
            ++currentPos;
            continue; }
        else {
            if (itr->first != key) { break; }
            thisResult->push_back(line);
            itr = std::next(itr);
            ++currentPos;
            pos = itr->second;
        }
    }
    file.close();
}

void MainWindow::textualSearchThread(ptrvecstrvecptr_t results, std::string word, size_t index) {
    auto thisDic = dictionaries->at(index);
    auto thisResult = results->at(index);
    for (auto && i : *thisDic) {
        std::string key = i.first;
        key = "<b>" + key + "</b>";
        for (auto && j : *i.second) {
            auto pos = j.find(word);
            if (pos != std::string::npos) {
                auto subsitute = "<b><span style=\"color:#ff0000;\">" + word + "</span></b>";
                j.replace(pos, word.length(), subsitute);
                key += ' ' + j;
                thisResult->push_back(key);
                key = i.first;
                key = "<b>" + key + "</b>";
                break;
            }
        }
    }
}

void MainWindow::textualSearch(std::string const & word) {
    auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
    for (auto i = 0; i < 2; ++i) {
        results->push_back(strvecptr_t(new strvec_t));
    }
    std::thread t0(&MainWindow::textualSearchThread, this, results, word, 0);
    std::thread t1(&MainWindow::textualSearchThread, this, results, word, 1);
    t0.join(); t1.join();
    auto resultSize = [&]() { size_t sz = 0; for (auto i : *results) { sz += i->size(); } return sz; }();
    if (resultSize == 0) {
        ui->results->setText("Word not found.");
        return;
    }
    std::string toprint;
    for (auto && i : * results) {
        for (auto && j : *i) {
            toprint += j + "<br><br>";
        }
    }
    toprint = "<p align=\"justify\">" + toprint + "</p>";
    ui->results->setHtml(toprint.c_str());
}


void MainWindow::printAll(std::string const & str) {
    auto word = str + ';';
    auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
    for (auto i = 0; i < 7; ++i) {
        results->push_back(strvecptr_t(new strvec_t));
    }
    std::thread t0(&MainWindow::printAllThread, this, results, word, 0);
    std::thread t1(&MainWindow::printAllThread, this, results, word, 1);
    std::thread t2(&MainWindow::printAllThread, this, results, word, 2);
    std::thread t3(&MainWindow::printAllThread, this, results, word, 3);
    std::thread t4(&MainWindow::printAllThread, this, results, word, 4);
    std::thread t5(&MainWindow::printAllThread, this, results, word, 5);
    std::thread t6(&MainWindow::printAllThread, this, results, word, 6);
    t0.join();
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join(); t6.join();
    auto resultSize = [&]() { size_t sz = 0; for (auto i : *results) { sz += i->size(); } return sz; }();
    if (resultSize == 0) {
        ui->results->setText("Word not found.");
        return;
    }
    std::string toprint;
    for (auto && i : * results) {
        for (auto && j : *i) {
            addStyleToResults(j);
            toprint += j + '\n';
        }
    }

    toprint = "<p align=\"center\"><table border=\"1\" cellpadding=\"20\">" + toprint + "</table></p>";
    ui->results->setHtml(toprint.c_str());
}

void MainWindow::printAllThread(ptrvecstrvecptr_t results, std::string word, size_t index) {
    auto thisDic = originals->at(index);
    auto thisResult = results->at(index);
    auto itr = thisDic->find(word);
    if (itr == thisDic->end()) { return; }
    std::string filename = ":/alphabet/sources/part" + std::to_string(index + 1);
    QFile file(filename.c_str());
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line;
    auto key = itr->first;
    auto pos = itr->second;
    auto currentPos = 0;
    while (std::getline(issfile, line)) {
        if (currentPos < pos) { ++currentPos; continue; }
        else {
            std::istringstream iss(line);
            std::string temp;
            iss >> temp;
            if (temp != key) { break; }
            thisResult->push_back(line);
            ++currentPos;
        }
    }
    file.close();
}

void MainWindow::printOneThread(ptrvecstrvecptr_t results, std::string word, std::string form, size_t index) {
    auto thisDic = originals->at(index);
    auto thisResult = results->at(index);
    auto itr = thisDic->find(word);
    if (itr == thisDic->end()) { return; }
    std::string filename = ":/alphabet/sources/part" + std::to_string(index + 1);
    QFile file(filename.c_str());
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line;
    auto key = itr->first;
    auto pos = itr->second;
    auto currentPos = 0;
    while (std::getline(issfile, line)) {
        if (currentPos < pos) { ++currentPos; continue; }
        else {
            std::istringstream iss(line);
            std::string temp;
            iss >> temp;
            if (temp == key) {
                if (line.find(form) != std::string::npos) {
//                    addStyleToResults(line);
                    thisResult->push_back(line);
                    break;
                }
                else { ++currentPos; }
            }
            else { break; }
        }
    }
    file.close();
}

void MainWindow::printOne(const std::string &arg1, const std::string &arg2) {
    ui->input->setPlaceholderText("Insert word here...");
    auto word = arg1 + ';';
    auto form = arg2 + ';';
    auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
    for (auto i = 0; i < 7; ++i) {
        results->push_back(strvecptr_t(new strvec_t));
    }
    std::thread t0(&MainWindow::printOneThread, this, results, word, form, 0);
    std::thread t1(&MainWindow::printOneThread, this, results, word, form, 1);
    std::thread t2(&MainWindow::printOneThread, this, results, word, form, 2);
    std::thread t3(&MainWindow::printOneThread, this, results, word, form, 3);
    std::thread t4(&MainWindow::printOneThread, this, results, word, form, 4);
    std::thread t5(&MainWindow::printOneThread, this, results, word, form, 5);
    std::thread t6(&MainWindow::printOneThread, this, results, word, form, 6);
    t0.join(); t1.join(); t2.join(); t3.join(); t4.join(); t5.join(); t6.join();
    auto resultSize = [&]() { size_t sz = 0; for (auto i : *results) { sz += i->size(); } return sz; }();
    if (resultSize == 0) {
        ui->results->setText("Word not found.");
        ui->input->clear();
        return;
    }
    std::string toprint;
    for (auto && i : * results) {
        for (auto && j : *i) {
            addStyleToResults(j);
            toprint += j + '\n';
        }
    }
    toprint = "<p align=\"center\"><table border=\"1\" cellpadding=\"20\">" + toprint + "</table></p>";
    ui->results->setHtml(toprint.c_str());
    typeTimes = 0;
    ui->input->clear();
}


void MainWindow::on_input_editingFinished()
{
    std::string word = ui->input->text().toStdString();
    if (flags[0] == 1) {
        findDefinition(word);
    }
    else if (flags[1] == 1) {
        textualSearch(word);
    }
    else if (flags[3] == 1 && inflectionReady) {
        findInflection(word);
    }
    else if (flags[4] == 1 && inflectionReady) {
        printAll(word);
    }
    else if (flags[5] == 1 && inflectionReady) {
        ++typeTimes;
        if (typeTimes > 0) {
            ui->input->clear();
            ui->input->setPlaceholderText("Insert form here...");
        }
        else {
            ui->input->clear();
            ui->input->setPlaceholderText("Insert word here...");
        }
    }
}

void MainWindow::on_options_itemClicked(QListWidgetItem *item)
{

    if (flags[0] == 1) {
        ui->input->setText(item->text());
        findDefinition(item->text().toStdString());
    }
    else if (flags[5] == 1) {
        ui->input->setText(item->text());
        printOneForm = item->text().toStdString();
        printOne(printOneWord, printOneForm);
    }
}

