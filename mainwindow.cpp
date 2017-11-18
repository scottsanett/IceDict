#include "QObject"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    Q_INIT_RESOURCE(resource);
    inflectionals = mapptrvecptr_t(new mapptrvec_t);
    definitions = mapptrvecptr_t(new mapptrvec_t);
    originals = mapptrvecptr_t(new mapptrvec_t);
    wordindex = strsetptr_t(new strset_t);
    dictionaries = strvecptrmapptrvecptr_t(new strvecptrmapptrvec_t);
    forms = strsetptr_t(new strset_t);
    for (int i = 0; i < 7; ++i) {
        inflectionals->push_back(mapptr_t(new map_t));
        originals->push_back(mapptr_t(new map_t));
    }
    for (int i = 0; i < 2; ++i) {
        definitions->push_back(mapptr_t(new map_t));
        dictionaries->push_back(strvecptrmapptr_t(new strvecptrmap_t));
    }

    importWordIndex();
    importForms();
    importOriginal();
    importDictionary();
    importInflections();
    ui->setupUi(this);
    this->setWindowTitle("Icelandic Dictionary");
    QIcon icon(":/alphabet/icon.ico");
    this->setWindowIcon(icon);
    ui->input->setPlaceholderText("Select a dictionary first...");
    ui->resultsTab->setDocumentMode(true);
    ui->resultsTab->setTabsClosable(true);
    ui->resultsTab->tabBar()->setMovable(true);
    ui->resultsTab->tabBar()->setAutoHide(true);
    ui->resultsTab->tabBar()->setExpanding(true);
    connect(ui->resultsTab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    addTab_clicked();
    QShortcut* searchShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
    QObject::connect(
                searchShortcut, &QShortcut::activated,
                this, &MainWindow::activateInput
                );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTab_clicked() {
    QTextBrowser * result = new QTextBrowser;
    result->setHtml(startScreen);
//    ui->input->clear();
    auto index = ui->resultsTab->addTab(result, "(empty)");
    ui->resultsTab->setCurrentIndex(index);
}

void MainWindow::closeTab(int index) {
    auto widget = ui->resultsTab->widget(index);
    if (ui->resultsTab->count() > 1) {
        ui->resultsTab->removeTab(index);
        if (widget) {
            delete widget;
        }
    }
}

void MainWindow::activateInput() {
    ui->input->activateWindow();
    ui->input->setFocus();
}

void MainWindow::clearResultFromDictionaries() {
    if (resultsFromDictionaries) {
        resultsFromDictionaries->clear();
        ui->inputLayout->removeWidget(resultsFromDictionaries);
        delete resultsFromDictionaries;
        resultsFromDictionaries = nullptr;
    }
}

std::string MainWindow::addStyleToResults(std::string line) {
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
    return line;
}

void MainWindow::search_norse_word()
{
    flags = {1, 0, 0, 0, 0, 0, 0};

    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
}

void MainWindow::search_icelandic_word()
{
    flags = {0, 0, 1, 0, 0, 0, 0};
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    onlineEntries.clear();
    ui->input->clear();

    ui->input->setPlaceholderText(tr("Insert word here..."));
}


void MainWindow::search_icelandic_text()
{
    flags = {0, 0, 0, 0, 0, 0, 1};
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    onlineEntries.clear();
    ui->input->clear();

    ui->input->setPlaceholderText(tr("Insert text here..."));
}


void MainWindow::search_norse_text()
{
    flags = {0, 1, 0, 0, 0, 0, 0};

    ui->input->setPlaceholderText("Insert text here...");
    ui->input->clear();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
}

void MainWindow::search_original()
{
    flags = {0, 0, 0, 1, 0, 0, 0};

    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
}

void MainWindow::search_all_inflections()
{
    flags = {0, 0, 0, 0, 1, 0, 0};

    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
}


void MainWindow::search_one_inflection()
{
    typeTimes = 0;
    flags = {0, 0, 0, 0, 0, 1, 0};

    stored.clear();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
}

std::string MainWindow::wordToWrite(std::string str) {
    for (auto && i : writeRules) {
        while (str.find(i.first) != std::string::npos) {
            auto pos = str.find(i.first);
            str.replace(pos, i.first.length(), i.second);
        }
    }
    return str;
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
    }
    wd.close();
}

void MainWindow::importInflections() {
    for (auto i = 1; i <= 7; ++i) {
        importInflectionsThread(inflectionals, i);
    }
}

/*import all the inflection forms and its position*/
void MainWindow::importInflectionsThread(mapptrvecptr_t mapvec, int i) {
    std::string filename = ":/alphabet/source_reverse_index/part" + to_string(i);
    QFile f(filename.c_str());
    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    int index = 0;
    auto thisMap = mapvec->at(i - 1);
    while (std::getline(file, line)) {
        thisMap->insert(std::make_pair(line, index));
        ++index;
    }
    f.close();
}

void MainWindow::importOriginalThread(mapptrvecptr_t mapvec, int i) {
    std::string filename = ":/alphabet/sources_index/part" + to_string(i);
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
        auto index_number = strtol(index.c_str(), 0, 10);
        thisMap->insert(std::make_pair(key, index_number));
//        thisMap->insert(std::make_pair(key, std::stoul(index)));
    }
    f.close();
}

void MainWindow::importOriginal() {
    for (auto i = 1; i <= 7; ++i) {
        importOriginalThread(originals, i);
    }
}

void MainWindow::importDictionary() {
    importDictionaryThread("zoega", 0);
    importDictionaryThread("vifgusson", 1);
}

void MainWindow::importDictionaryThread(std::string const name, int i) {
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

/**
 * @brief MainWindow::findDefinition
 * @param words
 * find a way to separate the suggested words that the overlapping entries that two dictionaries provide
 */

void MainWindow::findDefinition(std::string const & words) {
    std::string word = "<b>" + words + "</b>";
    auto upper = QString(words.c_str()).toUpper().toStdString();
    upper = "<b>" + upper + "</b>";

    auto zisspair = dictionaries->at(0)->equal_range(word);
    auto visspair = dictionaries->at(1)->equal_range(word);
    auto uisspair = dictionaries->at(1)->equal_range(upper);

    if ((zisspair.first == zisspair.second) && (visspair.first == visspair.second)) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setFontFamily("Perpetua");
        tabActive->setFontPointSize(20);
        tabActive->setText("Word not found.");
        return;
    }

    if (!resultsFromDictionaries) resultsFromDictionaries = new QListWidget;
    resultsFromDictionaries->setMaximumWidth(150);
    resultsFromDictionaries->setMaximumHeight(100);
    ui->inputLayout->addWidget(resultsFromDictionaries);
    QObject::connect(
                resultsFromDictionaries, &QListWidget::itemClicked,
                this, &MainWindow::resultsFromDictionaries_itemClicked
                );

    definitionResults.clear();
    for (auto itr = zisspair.first; itr != zisspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = *itr->second;
        definitionResults.push_back(std::make_pair(key, thisEntry));
    }
    for (auto itr = visspair.first; itr != visspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = *itr->second;
        definitionResults.push_back(std::make_pair(key, thisEntry));
    }
    for (auto itr = uisspair.first; itr != uisspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = *itr->second;
        definitionResults.push_back(std::make_pair(key, thisEntry));
    }

    QStringList alternatives;
    for (auto && i : definitionResults) {
        std::string key = i.first.c_str();
        key = key.substr(3, key.length() - 7);
        alternatives.push_back(key.c_str());
    }

    resultsFromDictionaries->clear();
    resultsFromDictionaries->addItems(alternatives);
    findDefinitionPrint(0);
}

void MainWindow::findDefinitionPrint(int index) {
    auto thisEntry = definitionResults.at(index);
    std::string key = thisEntry.first;
    std::string value;
    for (auto i : thisEntry.second) {
        value += i + '\n';
    }
    std::string display = key + ' ' + value;
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 20pt;\">" + display + "</span></p>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(display.c_str());
}

void MainWindow::findInflection(std::string const & word) {
    auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
    for (auto i = 0; i < 7; ++i) {
        results->push_back(strvecptr_t(new strvec_t));
    }
    for (auto i = 0; i < 7; ++i) {
        findInflectionThread(results, word, i);
    }
    auto resultSize = [&]() { int sz = 0; for (auto i : *results) { sz += i->size(); } return sz; }();
    if (resultSize == 0) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setFontPointSize(20);
        tabActive->setText("Word not found");
        return;
    }
    std::string toprint;
    for (auto && i : *results) {
        for (auto && j : *i) {
            j = addStyleToResults(j);
            toprint += j + "\n\n";
        }
    }
    toprint = "<span style=\"font-family: Segoe UI; font-size: 14pt;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint.c_str());
}

void MainWindow::findInflectionThread(ptrvecstrvecptr_t results, std::string word, int index) {
    auto thisDic = inflectionals->at(index);
    auto thisResult = results->at(index);
    auto itr = thisDic->find(word);
    if (itr == thisDic->end()) { return; }
    std::string filename = ":/alphabet/sources/part" + to_string(index + 1);
    QFile file(filename.c_str());
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line;
    auto key = itr->first;
    auto pos = itr->second;
    int currentPos = 0;
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

void MainWindow::textualSearchPrint(int index) {
    auto thisEntry = textualResults.at(index);
    std::string key = thisEntry.first;
    std::string display = thisEntry.second;
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 20pt;\">" + display + "</span></p>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(display.c_str());
}

void MainWindow::textualSearchThread(std::string word, int index) {
    auto thisDic = dictionaries->at(index);
    for (auto && i : *thisDic) {
        std::string key = i.first;
        for (auto && j : *i.second) {
            auto pos = j.find(word);
            if (pos != std::string::npos) {
                auto subsitute = "<b><span style=\"color:#ff0000;\">" + word + "</span></b>";
                j.replace(pos, word.length(), subsitute);
                std::string value = key + ' ' + j;
                textualResults.push_back(std::make_pair(key, value));
                break;
            }
        }
    }
}

void MainWindow::textualSearch(std::string const & word) {
    ui->input->clear();
//    ui->options->clear();
    textualSearchThread(word, 0);
    textualSearchThread(word, 1);
    if (textualResults.size() == 0) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setFontFamily("Perpetua");
        tabActive->setFontPointSize(20);
        tabActive->setText("Word not found.");
        return;
    }
    QStringList alternatives;
    for (auto i : textualResults) {
        std::string key = i.first;
        if (key.length() > 7) {
            key = key.substr(3, key.length() - 7);
            alternatives.push_back(key.c_str());
        }
    }
    ui->options->addItems(alternatives);
}


void MainWindow::printAll(std::string const & str) {
    ui->input->clear();
    ui->options->clear();
    auto word = str + ';';
    auto & results = resultsToPrint;
    for (auto i = 0; i < 7; ++i) {
        printAllThread(word, i);
    }
    auto resultSize = [&]() { int sz = 0; for (auto i : results) { sz += i.second.size(); } return sz; }();
    if (resultSize == 0) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setFontPointSize(20);
        tabActive->setText("Word not found.");
        return;
    }
    for (auto && i : results) {
            ui->options->addItem(i.first.c_str());
    }
    printAllPrint(0);
}

void MainWindow::printAllThread(std::string word, int index) {
    auto thisDic = originals->at(index);
    auto & thisResult = resultsToPrint; // pointer to a vector of
    auto range = thisDic->equal_range(word);
    auto count = thisDic->count(word);
    if (count == 0) { return; }
    std::string filename = ":/alphabet/sources/part" + to_string(index + 1);
    QFile file(filename.c_str());
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line, partOfSpeech;
    int currentPos = 0;
    for (auto itr = range.first; itr != range.second; ++itr) {
        auto key = itr->first;
        strvec_t thisEntry;
        auto pos = itr->second;
        while (std::getline(issfile, line)) {
            if (currentPos < pos) { ++currentPos; continue; }
            else {
                std::istringstream iss(line);
                std::string temp1;
                iss >> temp1;
                std::string temp2;
                iss >> temp2;
                if (currentPos == pos) { partOfSpeech = temp2; }
                if (temp1 != key) {
                    ++currentPos;
                    break;
                }
                else if (temp2 != partOfSpeech) {
//                    qDebug() << temp2.c_str();
                    ++currentPos;
                    partOfSpeech = temp2;
                    thisResult.push_back(std::make_pair(key, thisEntry));
                    thisEntry.clear();
                    thisEntry.push_back(line);
                    continue;
                }
                thisEntry.push_back(line);
                ++currentPos;
            }
        }
        thisResult.push_back(std::make_pair(key, thisEntry));
    }
    file.close();
}

void MainWindow::printAllPrint(int index) {
    auto thisResult = resultsToPrint[index];
    std::string toprint;
    for (auto i : thisResult.second) {
       std::string temp = addStyleToResults(i);
       toprint += temp;
    }
    toprint = "<span style=\"font-family: Segoe UI; font-size: 14pt;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint.c_str());
}

void MainWindow::printOneThread(ptrvecstrvecptr_t results, std::string word, std::string form, int index) {
    auto thisDic = originals->at(index);
    auto thisResult = results->at(index);
    auto range = thisDic->equal_range(word);
    auto count = thisDic->count(word);
    if (count == 0) { return; }
    std::string filename = ":/alphabet/sources/part" + to_string(index + 1);
    QFile file(filename.c_str());
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line;
    int currentPos = 0;
    for (auto itr = range.first; itr != range.second; ++itr) {
        auto key = itr->first;
        auto pos = itr->second;
        while (std::getline(issfile, line)) {
            if (currentPos < pos) { ++currentPos; continue; }
            else {
                std::istringstream iss(line);
                std::string temp;
                iss >> temp;
                if (temp == key) {
                    if (line.find(form) != std::string::npos) {
                        thisResult->push_back(line);
                        ++currentPos;
                        break;
                    }
                    else { ++currentPos; }
                }
                else { break; }
            }
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
    for (auto i = 0; i < 7; ++i) {
        printOneThread(results, word, form, i);
    }
    auto resultSize = [&]() { int sz = 0; for (auto i : *results) { sz += i->size(); } return sz; }();
    if (resultSize == 0) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setFontPointSize(20);
        tabActive->setText("Word not found.");
//        ui->input->clear();
        return;
    }
    std::string toprint;
    for (auto && i : * results) {
        for (auto && j : *i) {
            j = addStyleToResults(j);
            toprint += j + '\n';
        }
    }
    toprint = "<span style=\"font-family: Segoe UI; font-size: 14pt;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint.c_str());
    typeTimes = 0;
//    ui->input->clear();
}


void MainWindow::on_input_textEdited(const QString &arg1)
{
    std::string word = arg1.toStdString();
    ui->options->clear();
    if (flags[0] == 1) // search icelandic on
    {
        definitionResults.clear();
        QStringList display;
        if (word.length() > 1) {
            for (auto && entry : *wordindex) {
                auto pos = entry.find(word);
                if (pos == 0) {
                    display.push_back(entry.c_str());
                }
            }
            ui->options->addItems(display);
        }
    }
    else if (flags[1] == 1) {
        textualResults.clear();
    }
    else if (flags[2] == 1) {
        onlineEntries.clear();
    }
    else if (flags[6] == 1) {
        onlineEntries.clear();
    }
    else if (flags[3] == 1) //query on
    {
        if (word.length() > 2) {
            QStringList display;
            ui->options->addItems(display);
        }
    }
    else if (flags[4] == 1) {
        resultsToPrint.clear();
    }
    else if (flags[5] == 1) {
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

void MainWindow::on_input_editingFinished()
{
    std::string word = ui->input->text().toStdString();
    if (flags[0] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word.c_str());
        findDefinition(word);
    }
    else if (flags[1] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word.c_str());
        ui->options->clear();
        textualSearch(word);
    }
    else if (flags[2] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word.c_str());
        ui->options->clear();
        onlineEntries.clear();
        inputted = word;
        onlineDefinition(word);
    }
    else if (flags[3] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word.c_str());
        findInflection(word);
    }
    else if (flags[4] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word.c_str());
        ui->options->clear();
        resultsToPrint.clear();
        printAll(word);
    }
    else if (flags[5] == 1 && word.length() > 0) {
        ++typeTimes;
        if (typeTimes > 0) {
            ui->input->clear();
            ui->input->setPlaceholderText("Insert form here...");
            std::string tag = word;
            ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word.c_str());
        }
        else {
            ui->input->clear();
            ui->input->setPlaceholderText("Insert word here...");
        }
    }
    else if (flags[6] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word.c_str());
        ui->options->clear();
        onlineEntries.clear();
        inputted = word;
        onlineText(word);
    }
}


void MainWindow::on_options_itemClicked(QListWidgetItem *item)
{
    auto itemText = item->text();
    auto tag = itemText.toStdString();

    if (flags[0] == 1) {
        ui->input->setText(itemText);
//        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        on_input_editingFinished();
    }

    else if (flags[1] == 1) {
        ui->input->setText(itemText);
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        if (textualResults.size() == 0) {
            textualSearch(tag);
        }
        else {
            int index = ui->options->currentRow();
            textualSearchPrint(index);
        }
    }

    else if (flags[2] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        std::string url;
        auto itr = onlineEntries.find(tag);
        if (itr != onlineEntries.end()) {
            url = itr->second;
        }
        downloadPage(url);
    }
    else if (flags[4] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->clear();
        int index = ui->options->currentRow();
        printAllPrint(index);
// a function that prints according to the index.
    }
    else if (flags[5] == 1) {
        ui->options->clear();
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        ui->input->setText(itemText);
        printOne(printOneWord, tag);
    }
    else if (flags[6] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        std::string url;
        auto itr = onlineEntries.find(tag);
        if (itr != onlineEntries.end()) {
            url = itr->second;
        }
        downloadPage(url);
    }
}


void MainWindow::resultsFromDictionaries_itemClicked(QListWidgetItem * item) {
    auto itemText = item->text();
    auto tag = itemText.toStdString();
    if (flags[0] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());

        if (definitionResults.size() == 0) {
            findDefinition(tag);
        }
        else {
            int index = resultsFromDictionaries->currentRow();
            findDefinitionPrint(index);
        }
    }
}

void MainWindow::downloadPage(std::string url) {
    ui->input->clear();
    QUrl pageUrl(url.c_str());
    pageControl = new PageDownloader(pageUrl, this);
    connect(pageControl, SIGNAL(downloaded()), this, SLOT(loadPage()));
}

void MainWindow::loadPage() {
    QByteArray qPage = (pageControl->downloadedData());
    QString str = QString::fromLatin1(qPage);
    webpage = str.toStdString();
    if (parsePage()) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        webpage = "<span style=\"font-family: Perpetua; font-size: 20pt;\">" + webpage + "</span>";
        tabActive->setHtml(webpage.c_str());
        webpage.clear();
    }
}

void MainWindow::onlineText(const std::string & word) {
    std::string newWord = wordToWrite(word);
    std::string url = textUrl1 + newWord + textUrl2;
    downloadPage(url.c_str());
}

void MainWindow::onlineDefinition(const std::string &word) {
    std::string newWord = wordToWrite(word);
    std::string url = writeUrl1 + newWord + writeUrl2;
    downloadPage(url.c_str());
}

bool MainWindow::parsePage() {
    if (webpage.find("produced no results.") != std::string::npos) {
        auto pos = webpage.find("<h3>While searching in Icelandic Online Dictionary and Readings</h3>");
        if (pos != std::string::npos) webpage = webpage.substr(pos, webpage.length() - pos);
        pos = webpage.find("<div class=\"mainBackground\">");
        if (pos != std::string::npos) webpage = webpage.substr(0, pos);
        return true;
    }
    else if (webpage.find("<div class=\"results\">") != std::string::npos) {
        auto pos = webpage.find("<div class=\"results\">");
        if (pos != std::string::npos) webpage = webpage.substr(pos, webpage.length() - pos);
        pos = webpage.find("</div> <!-- results -->");
        if (pos != std::string::npos) webpage = webpage.substr(0, pos);
        std::vector<std::string> toBeDeleted = {
            "<div class=\"results\">", "<div class=\"nestlevel\">", "<span class=\"lemma\">",
            "<small><sup>", "</sup></small>", "</a></span>", "<span class=\"pos\">", "</span>",
            "</div>", "<!-- results -->", "<a href=\""
        };
        auto results = webpage;
        for (auto && i : toBeDeleted) {
            while (results.find(i) != std::string::npos) {
                auto pos = results.find(i);
                if (pos != std::string::npos) { results.erase(pos, i.length()); }
            }
        }
        std::istringstream iss(results);
        std::vector<std::string> entries;
        std::string line;
        while (std::getline(iss, line)) {
            if (line.length() > 0) {
                pos = line.find("\">");
                if (pos != std::string::npos) {
                    line.replace(pos, 2, " ");
                }
                entries.push_back(line);
            }
        }
        for (auto i : entries) {
            std::istringstream iss(i);
            std::string link, key;
            iss >> link;
            link = "http://digicoll.library.wisc.edu" + link;
            std::string temp;
            iss >> key;
            if (key.back() == ',') {
                std::string temp;
                iss >> temp;
                key += ' ' + temp;
            }
            onlineEntries.insert(std::make_pair(key, link));
        }
        QStringList alternatives;
        for (auto && i : onlineEntries) {
            alternatives.push_back(i.first.c_str());
        }
        ui->options->addItems(alternatives);
        return false;
    }
    else {
        auto pos = webpage.find("<div class=\"entry\">");
        if (pos != std::string::npos) { webpage = webpage.substr(pos, webpage.length() - pos); }
        pos = webpage.find("</div><!-- entry -->");
        if (pos != std::string::npos) { webpage = webpage.substr(0, pos); }
        pos = webpage.find("<span class=\"lemma\">");
        if (pos != std::string::npos) {
            std::string tag = "<span class=\"lemma\">";
            webpage.replace(pos, tag.length(), "<span style=\" font-weight:600; font-size:24pt;\" class=\"lemma\">");
        }
        while (webpage.find("<span class=\"orth\">") != std::string::npos) {
            std::string tag = "<span class=\"orth\">";
            auto pos = webpage.find(tag);
            if (pos != std::string::npos) {
                webpage.replace(pos, tag.length(), "<span style=\" font-weight:600;\" class=\"orth\">");
            }
        }
        while (webpage.find("<span class=\"trans\">") != std::string::npos) {
            std::string tag = "<span class=\"trans\">";
            auto pos = webpage.find(tag);
            if (pos != std::string::npos) {
                webpage.replace(pos, tag.length(), "<span style=\" font-style:italic;\" class=\"trans\">");
            }
        }
        if (flags[6] == 1) {
            auto pos = webpage.find(inputted);
            if (pos != std::string::npos) {
                webpage.replace(pos, inputted.length(), "<span style=\" font-weight:600; color:#ff0000;\">" + inputted + "</span>");
            }
        }
        return true;
    }
}

void MainWindow::on_actionMinimize_triggered()
{
    QWidget::setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_actionFullscreen_triggered()
{
    if (windowState() != Qt::WindowFullScreen) {
        QWidget::setWindowState(Qt::WindowFullScreen);
    }
    else {
        QWidget::setWindowState(Qt::WindowNoState);
    }
}

void MainWindow::on_actionNew_Tab_triggered()
{
    addTab_clicked();
}

void MainWindow::on_actionClose_Tab_triggered()
{
    if (ui->resultsTab->count() > 1) {
        auto activeTabIndex = ui->resultsTab->currentIndex();
        ui->resultsTab->removeTab(activeTabIndex);
    }
}


void MainWindow::on_actionModern_Icelandic_triggered()
{
    clearResultFromDictionaries();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_icelandic_word();
    ui->statusBar->showMessage("Search definitions for Modern Icelandic word (online)");
}


void MainWindow::on_actionEnglish_Modern_Icelandic_triggered()
{
    clearResultFromDictionaries();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_icelandic_text();
    ui->statusBar->showMessage("Search text in Modern Icelandic dictionary (online)");
}

void MainWindow::on_actionOld_Icelandic_English_triggered()
{
    clearResultFromDictionaries();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_norse_word();
    ui->statusBar->showMessage("Search definitions for Old Icelandic word");
}


void MainWindow::on_actionOld_Icelandic_Text_Search_triggered()
{
    clearResultFromDictionaries();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_norse_text();
    ui->statusBar->showMessage("Search text in Old Icelandic dictionaries");
}

void MainWindow::on_actionSearch_Inflections_triggered()
{
    clearResultFromDictionaries();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_original();
    ui->statusBar->showMessage("Search all inflection names of a word form");
}

void MainWindow::on_actionList_All_Forms_triggered()
{
    clearResultFromDictionaries();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_all_inflections();
    ui->statusBar->showMessage("List all inflections of a word");
}

void MainWindow::on_actionList_One_Specific_Form_triggered()
{
    clearResultFromDictionaries();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_one_inflection();
    ui->statusBar->showMessage("Search one specific inflectional form of a word");
}
