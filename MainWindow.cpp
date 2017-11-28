#include "QObject"
#include "MainWindow.hpp"
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

    for (int i = 0; i < 8; ++i) {
        inflectionals->push_back(mapptr_t(new map_t));
        originals->push_back(mapptr_t(new map_t));
    }

    for (int i = 0; i < 2; ++i) {
        definitions->push_back(mapptr_t(new map_t));
        dictionaries->push_back(strvecptrmapptr_t(new strvecptrmap_t));
    }

    importWordIndex();
    importDictionary();
    importOriginal();
    importInflections();
    ui->setupUi(this);
    this->setWindowTitle("IceDict");
    QIcon icon(":/alphabet/icon.ico");
    this->setWindowIcon(icon);
    ui->resultsTab->setDocumentMode(true);
    ui->resultsTab->setTabsClosable(true);
    ui->resultsTab->tabBar()->setMovable(true);
    ui->resultsTab->tabBar()->setAutoHide(true);
    ui->resultsTab->tabBar()->setExpanding(true);
#ifndef __APPLE__
    ui->centralLayout->setMargin(5);
#endif
    ui->statusBar->hide();
    connect(ui->resultsTab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    addTab_clicked();
    QShortcut* searchShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
    QObject::connect(
                searchShortcut, &QShortcut::activated,
                this, &MainWindow::activateInput
                );
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::addTab_clicked() {
    auto currentTab = std::make_shared<Pimpl>();
    currentTab->centralLayout = new QVBoxLayout;
    currentTab->mainSplitter = new QSplitter;

#ifdef __APPLE__
    currentTab->centralLayout->setMargin(0);
    currentTab->mainSplitter->setHandleWidth(0);
#endif

    tabIndices.insert(std::make_pair(currentTab->mainSplitter, currentTab));
    currentTab->centralLayout->addWidget(currentTab->mainSplitter);
    currentTab->inputLayout = new QSplitter;

#ifdef __APPLE__
    currentTab->inputLayout->setHandleWidth(0);
    currentTab->inputLayout->setFrameStyle(QFrame::NoFrame);
#endif

    currentTab->inputLayout->setOrientation(Qt::Vertical);
    currentTab->mainSplitter->addWidget(currentTab->inputLayout);

    currentTab->input = new QLineEdit;
    currentTab->input->setPlaceholderText("Select a dictionary first...");

#ifdef __APPLE__
    currentTab->input->setMaximumHeight(25);
    currentTab->input->setMinimumWidth(150);
    currentTab->input->setMaximumWidth(200);
    currentTab->input->setFrame(false);
    currentTab->input->setStyleSheet("font-family: Segoe UI; font-size: 13px");
#else
    currentTab->input->setMaximumHeight(40);
    currentTab->input->setMinimumWidth(250);
    currentTab->input->setMaximumWidth(300);
    currentTab->input->setStyleSheet("font-family: Segoe UI; font-size: 25px");
#endif

    currentTab->input->setEnabled(false);
    currentTab->inputLayout->addWidget(currentTab->input);
    QObject::connect(currentTab->input, &QLineEdit::textEdited,
                     this, &MainWindow::onInputTextEdited);
    QObject::connect(currentTab->input, &QLineEdit::editingFinished,
                     this, &MainWindow::onInputEditingFinished);

    currentTab->options = new QListWidget;

#ifdef __APPLE__
    currentTab->options->setMinimumWidth(150);
    currentTab->options->setMaximumWidth(200);
    currentTab->options->setFrameStyle(QFrame::NoFrame);
    currentTab->options->setStyleSheet("font-family: Segoe UI; font-size: 13px");
#else
    currentTab->options->setMinimumWidth(250);
    currentTab->options->setMaximumWidth(300);
    currentTab->options->setStyleSheet("font-family: Segoe UI; font-size: 25px");
#endif

    currentTab->options->setEnabled(false);
    currentTab->inputLayout->addWidget(currentTab->options);
    QObject::connect(currentTab->options, &QListWidget::itemClicked,
                     this, &MainWindow::onOptionsItemClicked);

    currentTab->result = new QTextBrowser;
    currentTab->result->setHtml(startScreen);
    currentTab->result->setFrameStyle(QFrame::NoFrame);
    currentTab->result->setContextMenuPolicy(Qt::CustomContextMenu);
    currentTab->mainSplitter->addWidget(currentTab->result);
    auto index = ui->resultsTab->addTab(currentTab->mainSplitter, "(empty)");
    QObject::connect(currentTab->result, &QTextBrowser::customContextMenuRequested,
                     this, &MainWindow::onResultContextMenuRequested);

    ui->resultsTab->setCurrentIndex(index);
}

void MainWindow::closeTab(int index) {
    auto currentSplitter = ui->resultsTab->currentWidget();
    if (tabIndices.find(currentSplitter) != tabIndices.end())
        tabIndices.erase(currentSplitter);
    auto widget = ui->resultsTab->widget(index);
    if (ui->resultsTab->count() > 1) {
        ui->resultsTab->removeTab(index);
        if (widget) delete widget;
    }
}

void MainWindow::activateInput() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->activateWindow();
    currentTab->input->setFocus();
}

void MainWindow::initializeResultFromDictionaries() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->resultsFromDictionaries)
        currentTab->resultsFromDictionaries = new QListWidget;

#ifdef __APPLE__
    currentTab->resultsFromDictionaries->setMaximumWidth(200);
    currentTab->resultsFromDictionaries->setMaximumHeight(150);
    currentTab->resultsFromDictionaries->setStyleSheet("font-family: Segoe UI; font-size: 13px");
#else
    currentTab->resultsFromDictionaries->setMaximumWidth(300);
    currentTab->resultsFromDictionaries->setMaximumHeight(200);
    currentTab->resultsFromDictionaries->setStyleSheet("font-family: Segoe UI; font-size: 25px");
#endif

    currentTab->resultsFromDictionaries->setFrameStyle(QFrame::NoFrame);
    currentTab->inputLayout->addWidget(currentTab->resultsFromDictionaries);
    QObject::connect(
                currentTab->resultsFromDictionaries, &QListWidget::itemClicked,
                this, &MainWindow::resultsFromDictionariesItemClicked
                );
}

void MainWindow::clearResultFromDictionaries() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (currentTab->resultsFromDictionaries) {
        currentTab->resultsFromDictionaries->clear();
        delete currentTab->resultsFromDictionaries;
        currentTab->resultsFromDictionaries = nullptr;
    }
#ifdef __APPLE__
        currentTab->perpetuaFontSize = 20;
        currentTab->segoeFontSize = 14;
#else
        currentTab->perpetuaFontSize = 40;
        currentTab->segoeFontSize = 28;
#endif
}

void MainWindow::initializeInflectionForms() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms)
        currentTab->inflectionForms = new TreeWidget;
    currentTab->inflectionForms->setHeaderLabel("Inflections");
#ifdef __APPLE__
    currentTab->inflectionForms->setMaximumWidth(300);
    currentTab->inflectionForms->setMinimumHeight(450);
    currentTab->inflectionForms->setFrameStyle(QFrame::NoFrame);
    currentTab->inflectionForms->setStyleSheet("font-family: Segoe UI; font-size: 13px");
#else
    currentTab->inflectionForms->setMaximumWidth(350);
    currentTab->inflectionForms->setMinimumHeight(500);
    currentTab->inflectionForms->setStyleSheet("font-family: Segoe UI; font-size: 28px");
#endif

    currentTab->inputLayout->addWidget(currentTab->inflectionForms);
}

void MainWindow::clearInflectionForms() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (currentTab->inflectionForms) {
        currentTab->inflectionForms->clear();
        delete currentTab->inflectionForms;
        currentTab->inflectionForms = nullptr;
    }
}

QString MainWindow::addStyleToResults(QString line) {
    std::string result;
    std::istringstream iss(line.toStdString());
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
    result = "<tr>" + key + arg2 + arg3 + arg4 + "</tr>";
    return result.c_str();
}


void MainWindow::search_icelandic_word()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->flags = {{1, 0, 0, 0, 0, 0}};
    currentTab->input->clear();
    currentTab->onlineEntries.clear();
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert word here...");
}


void MainWindow::search_icelandic_text()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->flags = {{0, 1, 0, 0, 0, 0}};
    currentTab->input->clear();
    currentTab->onlineEntries.clear();
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert text here...");
}

void MainWindow::search_norse_word()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->flags = {{0, 0, 1, 0, 0, 0}};
    currentTab->input->setPlaceholderText(tr("Insert word here..."));
    currentTab->input->clear();
    currentTab->result->clear();
    currentTab->options->clear();
}

void MainWindow::search_norse_text()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->flags = {{0, 0, 0, 1, 0, 0}};
    currentTab->input->clear();
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert text here...");
}

void MainWindow::search_original()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->flags = {{0, 0, 0, 0, 1, 0}};
    currentTab->input->clear();
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert word here...");
}

void MainWindow::search_all_inflections()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->flags = {{0, 0, 0, 0, 0, 1}};
    currentTab->input->clear();
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert word here...");
}



QString MainWindow::wordToWrite(QString arg) {
    auto str = arg;
    for (auto && i : writeRules) {
        while (str.contains(i.first)) {
            auto pos = str.indexOf(i.first);
            str.replace(pos, i.first.length(), i.second);
        }
    }
    return str;
}

/* import the index for all words in the two dictionaries */

void MainWindow::importWordIndex() {
    QFile wd(":/alphabet/wordindex");

    wd.open(QIODevice::ReadOnly);
    QString file = wd.readAll();
    std::istringstream wordindexfile(file.toStdString());
    std::string line;
    while (std::getline(wordindexfile, line)) {
        wordindex->insert(line.c_str());
    }

    wd.close();
}

void MainWindow::importInflections() {
    for (size_t i = 1; i <= 8; ++i) {
        importInflectionsThread(inflectionals, i);
    }
}


/*import all the inflection forms and its position*/
void MainWindow::importInflectionsThread(mapptrvecptr_t mapvec, size_t i) {
    QString filename = ":/alphabet/source_reverse_index/part" + QString(to_string(i).c_str());
    QFile f(filename);

    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    int index = 0;
    auto thisMap = mapvec->at(i - 1);
    while (std::getline(file, line)) {
        thisMap->insert(std::make_pair(line.c_str(), index));
        ++index;
    }

    f.close();
}

void MainWindow::importOriginal() {
    for (size_t i = 1; i <= 8; ++i) {
        importOriginalThread(originals, i);
    }
}

void MainWindow::importOriginalThread(mapptrvecptr_t mapvec, size_t i) {
    QString filename = QString(":/alphabet/source_index/part") + to_string(i).c_str();
    QFile f(filename);

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
        thisMap->insert(std::make_pair(key.c_str(), index_number));
    }

    f.close();
}

void MainWindow::importDictionary() {
    importDictionaryThread("zoega", 0);
    importDictionaryThread("vifgusson", 1);
}

void MainWindow::importDictionaryThread(QString const name, size_t i) {
    QString filename = ":/alphabet/" + name;
    QFile f(filename);


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
               entry->push_back(sense.c_str());
               sense = "";
            }
        }
        thisMap->insert(std::make_pair(key.c_str(), entry));
    }


    f.close();
}

/**
 * @brief MainWindow::findDefinition
 * @param words
 * find a way to separate the suggested words that the overlapping entries that two dictionaries provide
 */

void MainWindow::findDefinition(QString const & words) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());

    QString word = "<b>" + words + "</b>";
    auto upper = QString(words).toUpper();
    upper = "<b>" + upper + "</b>";

    auto zisspair = dictionaries->at(0)->equal_range(word);
    auto visspair = dictionaries->at(1)->equal_range(word);
    auto uisspair = dictionaries->at(1)->equal_range(upper);

    if ((zisspair.first == zisspair.second) && (visspair.first == visspair.second))
    {
        currentTab->result->setFontFamily("Perpetua");
        currentTab->result->setFontPointSize(20);
        currentTab->result->setText("Word not found");
        return;
    }

    initializeResultFromDictionaries();

    currentTab->definitionResults.clear();
    for (auto itr = zisspair.first; itr != zisspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = *itr->second;
        currentTab->definitionResults.push_back(std::make_pair(key, thisEntry));
    }
    for (auto itr = visspair.first; itr != visspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = *itr->second;
        currentTab->definitionResults.push_back(std::make_pair(key, thisEntry));
    }
    for (auto itr = uisspair.first; itr != uisspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = *itr->second;
        currentTab->definitionResults.push_back(std::make_pair(key, thisEntry));
    }

    QStringList alternatives;
    for (auto && i : currentTab->definitionResults) {
        QString key = i.first;
        key = key.mid(3, key.length() - 7);
        alternatives.push_back(key);
    }

    currentTab->resultsFromDictionaries->clear();
    currentTab->resultsFromDictionaries->addItems(alternatives);
    findDefinitionPrint(0);
}

void MainWindow::findDefinitionPrint(size_t index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto thisEntry = currentTab->definitionResults.at(index);

    QString key = thisEntry.first;
    QString value;
    for (auto i : thisEntry.second) {
        value += i + '\n';
    }
    QString display = key + ' ' + value;

#ifdef __APPLE__
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 20px;\">" + display + "</span></p>";
#else
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 40px;\">" + display + "</span></p>";
#endif

    currentTab->result->setHtml(display);
}

void MainWindow::findInflection(QString const & word) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
    for (auto i = 0; i < 8; ++i) {
        results->push_back(strvecptr_t(new strvec_t));
    }

    for (size_t i = 0; i < 8; ++i) {
        findInflectionThread(results, word, i);
    }

    auto resultSize = [&]() { int sz = 0; for (auto i : *results) { sz += i->size(); } return sz; }();
    if (resultSize == 0) {
        currentTab->result->setFontPointSize(20);
        currentTab->result->setText("Word not found");
        return;
    }

    QString toprint;
    for (auto && i : *results) {
        for (auto && j : *i) {
            j = addStyleToResults(j);
            toprint += j + "\n\n";
        }
    }

#ifdef __APPLE__
    toprint = "<span style=\"font-family: Segoe UI; font-size: 14px;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
#else
        toprint = "<span style=\"font-family: Segoe UI; font-size: 28px;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
#endif

    currentTab->result->setHtml(toprint);
}

void MainWindow::findInflectionThread(ptrvecstrvecptr_t results, QString word, size_t index) {
    auto thisDic = inflectionals->at(index);
    auto thisResult = results->at(index);
    auto itr = thisDic->find(word);
    if (itr == thisDic->end()) { return; }
    QString filename = QString(":/alphabet/source/part") + to_string(index + 1).c_str();
    QFile file(filename);
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
            thisResult->push_back(line.c_str());
            itr = std::next(itr);
            ++currentPos;
            pos = itr->second;
        }
    }

    file.close();
}

void MainWindow::textualSearchPrint(size_t index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto thisEntry = currentTab->textualResults.at(index);
    QString key = thisEntry.first;
    QString display = thisEntry.second;
#ifdef __APPLE__
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 20px;\">" + display + "</span></p>";
#else
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 40px;\">" + display + "</span></p>";
#endif
    currentTab->result->setHtml(display);
}

void MainWindow::textualSearchThread(QString word, size_t index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto thisDic = dictionaries->at(index);
    for (auto && i : *thisDic) {
        QString key = i.first;
        QString value;
        bool found = false;
        for (auto j : *i.second) {
            auto pos = j.indexOf(word);
            if (pos != -1) {
                found = true;
                auto subsitute = "<b><span style=\"color:#ff0000;\">" + word + "</span></b>";
                j.replace(pos, word.length(), subsitute);
            }
            value += ' ' + j;
        }
        value = key + ' ' + value;
        if (found) currentTab->textualResults.push_back(std::make_pair(key, value));
    }
}

void MainWindow::textualSearch(QString const & word) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->clear();

    textualSearchThread(word, 0);
    textualSearchThread(word, 1);

    if (currentTab->textualResults.size() == 0) {
        currentTab->result->setFontFamily("Perpetua");
        currentTab->result->setFontPointSize(20);
        currentTab->result->setText("Word not found.");
        return;
    }

    QStringList alternatives;
    for (auto i : currentTab->textualResults) {
        QString key = i.first;
        if (key.length() > 7) {
            key = key.mid(3, key.length() - 7);
            alternatives.push_back(key);
        }
    }

    currentTab->options->addItems(alternatives);
}


void MainWindow::printAll(QString const & str) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->clear();
    currentTab->options->clear();

    auto word = str + ';';
    auto & results = currentTab->resultsToPrint;
    for (size_t i = 0; i < 8; ++i) {
        printAllThread(word, i);
    }

    auto resultSize = [&]() { int sz = 0; for (auto i : results) { sz += i.second.size(); } return sz; }();

    if (resultSize == 0) {
        currentTab->result->setFontPointSize(20);
        currentTab->result->setText("Word not found");
        return;
    }
    for (auto && i : results) {
        currentTab->options->addItem(i.first);
    }
    printAllPrint(0);
}

void MainWindow::printAllThread(QString word, size_t index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto & thisDic = originals->at(index);
    auto & thisResult = currentTab->resultsToPrint;
    auto range = thisDic->equal_range(word);
    auto count = thisDic->count(word);
    if (count == 0) { return; }
    QString filename = QString(":/alphabet/source/part") + to_string(index + 1).c_str();
    QFile file(filename);
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
                if (temp1.c_str() != key) {
                    ++currentPos;
                    break;
                }
                else if (temp2 != partOfSpeech) {
                    ++currentPos;
                    partOfSpeech = temp2;
                    thisResult.push_back(std::make_pair(key, thisEntry));
                    thisEntry.clear();
                    thisEntry.push_back(line.c_str());
                    continue;
                }
                thisEntry.push_back(line.c_str());
                ++currentPos;
            }
        }

        thisResult.push_back(std::make_pair(key, thisEntry));
    }
    file.close();
}

void MainWindow::printAllPrint(size_t index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto thisResult = currentTab->resultsToPrint.at(index);

    currentTab->inflStruct.clear();
    currentTab->inflSelectResult.clear();
    currentTab->inflSelectResult = thisResult.second;

    QString toprint;

    for (auto i : thisResult.second) {
       QString temp = addStyleToResults(i);
       toprint += temp;
    }

    clearInflectionForms();
    initializeInflectionForms();
    fillInflectionForms(toprint);
#ifdef __APPLE__
    toprint = "<span style=\"font-family: Segoe UI; font-size: 14px;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
#else
        toprint = "<span style=\"font-family: Segoe UI; font-size: 28px;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
#endif
    currentTab->result->setHtml(toprint);
}


void MainWindow::onInputTextEdited(const QString &arg1)
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QString word = arg1;
    currentTab->options->clear();

    if (currentTab->flags[0] == 1) {
        currentTab->onlineEntries.clear();
    }
    else if (currentTab->flags[1] == 1) {
        currentTab->onlineEntries.clear();
    }
    else if (currentTab->flags[2] == 1) {
        currentTab->definitionResults.clear();
        QStringList display;
        if (word.length() > 1) {
            for (auto && entry : *wordindex) {
                auto pos = entry.indexOf(word);
                if (pos == 0) {
                    display.push_back(entry);
                }
            }
            currentTab->options->addItems(display);
        }
    }
    else if (currentTab->flags[3] == 1) {
        currentTab->textualResults.clear();
    }
    else if (currentTab->flags[4] == 1) {
        if (word.length() > 2) {
            QStringList display;
            currentTab->options->addItems(display);
        }
    }
    else if (currentTab->flags[5] == 1) {
        currentTab->resultsToPrint.clear();
    }
}


void MainWindow::onInputEditingFinished()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QString word = currentTab->input->text();

    if (currentTab->flags[0] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        currentTab->options->clear();
        currentTab->onlineEntries.clear();
        currentTab->inputted = word;
        onlineDefinition(word);
    }
    else if (currentTab->flags[1] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        currentTab->options->clear();
        currentTab->onlineEntries.clear();
        currentTab->inputted = word;
        onlineText(word);
    }
    else if (currentTab->flags[2] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        findDefinition(word);
    }
    else if (currentTab->flags[3] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        currentTab->options->clear();
        textualSearch(word);
    }
    else if (currentTab->flags[4] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        findInflection(word);
    }
    else if (currentTab->flags[5] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        currentTab->options->clear();
        currentTab->resultsToPrint.clear();
        printAll(word);
    }
}


void MainWindow::onOptionsItemClicked(QListWidgetItem *item)
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto itemText = item->text();
    auto tag = itemText;

    if (currentTab->flags[0] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        QString url;
        auto itr = currentTab->onlineEntries.find(tag);
        if (itr != currentTab->onlineEntries.end()) {
            url = itr->second;
        }
        downloadPage(url);
    }
    else if (currentTab->flags[1] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        QString url;
        auto itr = currentTab->onlineEntries.find(tag);
        if (itr != currentTab->onlineEntries.end()) {
            url = itr->second;
        }
        downloadPage(url);
    }
    else if (currentTab->flags[2] == 1) {
        currentTab->input->setText(itemText);
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        onInputEditingFinished();
    }
    else if (currentTab->flags[3] == 1) {
        currentTab->input->setText(itemText);
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        if (currentTab->textualResults.size() == 0) {
            textualSearch(tag);
        }
        else {
            size_t index = currentTab->options->currentRow();
            textualSearchPrint(index);
        }
    }
    else if (currentTab->flags[5] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        currentTab->result->clear();
        size_t index = currentTab->options->currentRow();
        printAllPrint(index);
    }
}


void MainWindow::resultsFromDictionariesItemClicked(QListWidgetItem * item) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto itemText = item->text();
    auto tag = itemText;
    if (currentTab->flags[2] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        if (currentTab->definitionResults.size() == 0) {
            findDefinition(tag);
        }
        else {
            size_t index = currentTab->resultsFromDictionaries->currentRow();
            findDefinitionPrint(index);
        }
    }
}

void MainWindow::downloadPage(QString url) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->clear();

    QUrl pageUrl(url);
    pageControl = new PageDownloader(pageUrl, this);
    QObject::connect(pageControl, SIGNAL(downloaded()), this, SLOT(loadPage()));
}

void MainWindow::loadPage() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QByteArray qPage = (pageControl->downloadedData());
    QString str = QString::fromLatin1(qPage);

    currentTab->webpage = str;
    currentTab->webpage = str;

    if (parsePage()) {
#ifdef __APPLE__
        currentTab->webpage = "<span style=\"font-family: Perpetua; font-size: 20px;\">" + currentTab->webpage + "</span>";
#else
        currentTab->webpage = "<span style=\"font-family: Perpetua; font-size: 40px;\">" + currentTab->webpage + "</span>";
#endif
        currentTab->result->setHtml(currentTab->webpage);
        currentTab->webpage.clear();
    }
}

void MainWindow::onlineText(const QString & word) {
    QString newWord = wordToWrite(word);
    QString url = textUrl1 + newWord + textUrl2;
    downloadPage(url);
}

void MainWindow::onlineDefinition(const QString &word) {
    QString newWord = wordToWrite(word);
    QString url = writeUrl1 + newWord + writeUrl2;
    downloadPage(url);
}

/**
 * @brief MainWindow::parsePage
 * needs revision
 * @return
 */
bool MainWindow::parsePage() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (currentTab->webpage.contains("produced no results.")) {
        auto pos = currentTab->webpage.indexOf("<h3>While searching in Icelandic Online Dictionary and Readings</h3>");
        if (pos != -1) currentTab->webpage = currentTab->webpage.mid(pos, currentTab->webpage.length() - pos);
        pos = currentTab->webpage.contains("<div class=\"mainBackground\">");
        if (pos != -1) currentTab->webpage = currentTab->webpage.mid(0, pos);
        return true;
    }
    else if (currentTab->webpage.contains("<div class=\"results\">")) {
        auto pos = currentTab->webpage.indexOf("<div class=\"results\">");
        if (pos != -1) currentTab->webpage = currentTab->webpage.mid(pos, currentTab->webpage.length() - pos);
        pos = currentTab->webpage.indexOf("</div> <!-- results -->");
        if (pos != -1) currentTab->webpage = currentTab->webpage.mid(0, pos);
        QVector<QString> toBeDeleted = {
            "<div class=\"results\">", "<div class=\"nestlevel\">", "<span class=\"lemma\">",
            "<small><sup>", "</sup></small>", "</a></span>", "<span class=\"pos\">", "</span>",
            "</div>", "<!-- results -->", "<a href=\""
        };
        auto results = currentTab->webpage;
        for (auto && i : toBeDeleted) {
            while (results.contains(i)) {
                auto pos = results.indexOf(i);
                if (pos != -1) { results.remove(pos, i.length()); }
            }
        }

        std::istringstream iss(results.toStdString());
        QVector<QString> entries;
        std::string line;
        while (std::getline(iss, line)) {
            if (line.length() > 0) {
                pos = line.find("\">");
                if (pos != std::string::npos) {
                    line.replace(pos, 2, " ");
                }
                entries.push_back(line.c_str());
            }
        }
        for (auto i : entries) {
            std::istringstream iss(i.toStdString());
            std::string link, key;
            iss >> link;
            link = "http://digicoll.library.wisc.edu" + link;
            QString temp;
            iss >> key;
            if (*key.end() == ',') {
                std::string temp;
                iss >> temp;
                key += ' ' + temp;
            }
            currentTab->onlineEntries.insert(std::make_pair(key.c_str(), link.c_str()));
        }

        QStringList alternatives;
        for (auto && i : currentTab->onlineEntries) {
            alternatives.push_back(i.first);
        }
        currentTab->options->addItems(alternatives);
        return false;
    }
    else {
        auto pos = currentTab->webpage.indexOf("<div class=\"entry\">");
        if (pos != -1) { currentTab->webpage = currentTab->webpage.mid(pos, currentTab->webpage.length() - pos); }
        pos = currentTab->webpage.indexOf("</div><!-- entry -->");
        if (pos != -1) { currentTab->webpage = currentTab->webpage.mid(0, pos); }
        pos = currentTab->webpage.indexOf("<span class=\"lemma\">");
        if (pos != -1) {
            QString tag = "<span class=\"lemma\">";
            currentTab->webpage.replace(pos, tag.length(), "<span style=\" font-weight:600; font-size:24px;\" class=\"lemma\">");
        }
        while (currentTab->webpage.contains("<span class=\"orth\">")) {
            QString tag = "<span class=\"orth\">";
            auto pos = currentTab->webpage.indexOf(tag);
            if (pos != -1) {
                currentTab->webpage.replace(pos, tag.length(), "<span style=\" font-weight:600;\" class=\"orth\">");
            }
        }
        while (currentTab->webpage.contains("<span class=\"trans\">")) {
            QString tag = "<span class=\"trans\">";
            auto pos = currentTab->webpage.indexOf(tag);
            if (pos != -1) {
                currentTab->webpage.replace(pos, tag.length(), "<span style=\" font-style:italic;\" class=\"trans\">");
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
        closeTab(activeTabIndex);
    }
}


void MainWindow::on_actionModern_Icelandic_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();

    ui->actionZoom_In->setEnabled(false);
    ui->actionZoom_Out->setEnabled(false);

    ui->statusBar->showMessage("Search definitions for Modern Icelandic word (online)");

    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->setEnabled(true);
    currentTab->options->setEnabled(true);
    search_icelandic_word();
}


void MainWindow::on_actionEnglish_Modern_Icelandic_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();

    ui->actionZoom_In->setEnabled(false);
    ui->actionZoom_Out->setEnabled(false);
    ui->statusBar->showMessage("Search text in Modern Icelandic dictionary (online)");

    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->setEnabled(true);
    currentTab->options->setEnabled(true);
    search_icelandic_text();
}

void MainWindow::on_actionOld_Icelandic_English_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->setEnabled(true);
    currentTab->options->setEnabled(true);
    search_norse_word();
}


void MainWindow::on_actionOld_Icelandic_Text_Search_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->setEnabled(true);
    currentTab->options->setEnabled(true);
    search_norse_text();

    ui->statusBar->showMessage("Search text in Old Icelandic dictionaries");
}

void MainWindow::on_actionSearch_Inflections_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->setEnabled(true);
    currentTab->options->setEnabled(true);
    search_original();

    ui->statusBar->showMessage("Search all inflection names of a word form");
}

void MainWindow::on_actionList_All_Forms_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->setEnabled(true);
    currentTab->options->setEnabled(true);
    search_all_inflections();

    ui->statusBar->showMessage("List all inflections of a word");
}


void MainWindow::checkStateChanged(Qt::CheckState state, QVector<QString> const vec) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (state == Qt::CheckState::Checked) {
        if (currentTab->inflStruct.find(vec) == currentTab->inflStruct.end()) {
            currentTab->inflStruct.insert(vec);
        }
    }
    else if (state == Qt::CheckState::Unchecked) {
        currentTab->inflStruct.erase(currentTab->inflStruct.find(vec));
    }

/*
    for (auto && i : inflStruct) {
        qDebug() << i;
    }
*/

    auto resultVec = ParseCheckStateChangeInfo();

    QString toprint;
    for (auto i : resultVec) {
       QString temp = addStyleToResults(i);
       toprint += temp;
    }
#ifdef __APPLE__
    toprint = "<span style=\"font-family: Segoe UI; font-size: 14px;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
#else
    toprint = "<span style=\"font-family: Segoe UI; font-size: 28px;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
#endif
    currentTab->result->setHtml(toprint);
}

TreeWidgetItem * MainWindow::constructItem(QString label, TreeWidget * parent) {
    auto item = new TreeWidgetItem(parent);
    item->setText(0, label);
    item->setCheckState(0, Qt::Unchecked);
    QObject::connect(
                item, &TreeWidgetItem::checkStateChanged,
                this, &MainWindow::checkStateChanged
                );
    return item;
}

TreeWidgetItem * MainWindow::constructItem(QString label, TreeWidgetItem * parent) {
    auto item = new TreeWidgetItem(parent);
    item->setText(0, label);
    item->setCheckState(0, Qt::Unchecked);
    QObject::connect(
                item, &TreeWidgetItem::checkStateChanged,
                this, &MainWindow::checkStateChanged
                );
    return item;
}

template <bool D,
          Infl::Category cat,
          Infl::Forms ... args,
          class T>
void MainWindow::fillStructures(T * item, QString const & str) {
    if (!item) return;
    std::array<bool, sizeof... (args)> values;
    values = fillStructuresHelper<sizeof...(args), args...>(str);

    bool boolSum = false;
    for (auto && v : values) {
        if (v == true) { boolSum = true; break; }
    }
    if (boolSum == false) return;

    auto parent = constructItem(InflManager.categoryOf(cat), item);
    fillStructureItemConstructor<sizeof...(args), args...>(parent, values);
}


template <int Size, Infl::Forms ... Forms>
std::array<bool, Size> MainWindow::fillStructuresHelper(QString const & str) {
    std::array<bool, Size> result;
    auto values = {Forms...}; // initializer list
    unsigned index = 0;
    std::for_each(values.begin(), values.end(), [&](Infl::Forms n){
        result[index++] = InflManager.find(str, Infl::Short, n);
    });
    return result;
}

template <int Size, Infl::Forms ... Forms>
void MainWindow::fillStructureItemConstructor(TreeWidgetItem * item, std::array<bool, Size> const & array) {
    auto values = {Forms...};
    unsigned index = 0;
    std::for_each(values.begin(), values.end(), [&](Infl::Forms f){
        if (array.at(index++) == true) {
            constructItem(InflManager.nameOf(f), item);
        }
    });
}

void MainWindow::fillInflectionForms(QString const & str) {
    if (str.contains(PartOfSpeech.MasNoun)) {
        currentPOS = POS::Noun;
        fillNouns(str);
    }
    else if (str.contains(PartOfSpeech.FemNoun)) {
        currentPOS = POS::Noun;
        fillNouns(str);
    }
    else if (str.contains(PartOfSpeech.NetNoun)) {
        currentPOS = POS::Noun;
        fillNouns(str);
    }
    else if (str.contains(PartOfSpeech.Verb)) {
        currentPOS = POS::Verb;
        fillVerbs(str);
    }
    else if (str.contains(PartOfSpeech.Adjective)) {
        currentPOS = POS::Adjective;
        fillAdjectives(str);
    }
    else if (str.contains(PartOfSpeech.Adverb)) {
        currentPOS = POS::Adverb;
        fillAdverbs(str);
    }
    else if (str.contains(PartOfSpeech.Pronoun)) {
        currentPOS = POS::Pronoun;
        fillPronouns(str);
    }
    else if (str.contains(PartOfSpeech.Numerical)) {
        currentPOS = POS::Numerical;
        fillPronouns(str);
    }
    else ;
}

void MainWindow::fillVerbs(QString const & str) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms) return;

    auto none = constructItem("Non-Impersonal", currentTab->inflectionForms);
    fillStructures<false, TYPE_VOICE>(none, str);
    fillStructures<false, TYPE_MOOD>(none, str);
    fillStructures<false, TYPE_TENSE>(none, str);
    fillStructures<false, TYPE_PERSON>(none, str);
    fillStructures<false, TYPE_NUMBER>(none, str);

    bool findImpersonal = InflManager.find(str, Infl::Short, Infl::Impersonal);
    if (findImpersonal) {
        auto impersonal = constructItem(InflManager.nameOf(Infl::Impersonal), currentTab->inflectionForms);
        fillStructures<false, TYPE_VOICE>(impersonal, str);
        fillStructures<false, TYPE_MOOD>(impersonal, str);
        fillStructures<false, TYPE_TENSE>(impersonal, str);
        fillStructures<false, TYPE_PERSON>(impersonal, str);
        fillStructures<false, TYPE_NUMBER>(impersonal, str);
    }

    bool findInfinitive = InflManager.find(str, Infl::Short, Infl::Infinitive);
    if (findInfinitive) {
        auto infinitive = constructItem(InflManager.nameOf(Infl::Infinitive), currentTab->inflectionForms);
        fillStructures<false, TYPE_VOICE>(infinitive, str);
    }

    bool findImperative = InflManager.find(str, Infl::Short, Infl::Imperative);
    if (findImperative) {
        auto imperative = constructItem(InflManager.nameOf(Infl::Imperative), currentTab->inflectionForms);
        fillStructures<false, TYPE_IMPERAT_NUMBER>(imperative, str);
    }

    bool findSupine = InflManager.find(str, Infl::Short, Infl::Supine);
    if (findSupine) {
        auto supine = constructItem(InflManager.nameOf(Infl::Supine), currentTab->inflectionForms);
        fillStructures<false, TYPE_VOICE>(supine, str);
    }

    bool findPresParticiple = InflManager.find(str, Infl::Short, Infl::PresentParticiple);
    if (findPresParticiple) {
        auto participle = constructItem(InflManager.nameOf(Infl::PresentParticiple), currentTab->inflectionForms);
    }

    bool findPastParticiple = InflManager.find(str, Infl::Short, Infl::PastParticiple);
    if (findPastParticiple) {
        auto participle = constructItem(InflManager.nameOf(Infl::PastParticiple), currentTab->inflectionForms);
        fillStructures<false, TYPE_DEFINITENESS>(participle, str);
        fillStructures<false, TYPE_GENDER>(participle, str);
        fillStructures<false, TYPE_CASE>(participle, str);
        fillStructures<false, TYPE_NUMBER>(participle, str);
    }
}

void MainWindow::fillNouns(QString const & str) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms) return;
    fillStructures<true, TYPE_DEFINITENESS>(currentTab->inflectionForms, str);
    fillStructures<true, TYPE_CASE>(currentTab->inflectionForms, str);
    fillStructures<true, TYPE_NUMBER>(currentTab->inflectionForms, str);
}

void MainWindow::fillAdjectives(QString const & str) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms) return;

    bool findPositive = InflManager.find(str, Infl::Short, Infl::Positive);
    bool findComparat = InflManager.find(str, Infl::Short, Infl::Comparative);
    bool findSuperlat = InflManager.find(str, Infl::Short, Infl::Superlative);

    if (!(findPositive || findComparat || findSuperlat)) return;

    if (findPositive) {
        auto positive = constructItem("Positive", currentTab->inflectionForms);
        fillStructures<false, TYPE_DEFINITENESS>(positive, str);
        fillStructures<false, TYPE_GENDER>(positive, str);
        fillStructures<false, TYPE_CASE>(positive, str);
        fillStructures<false, TYPE_NUMBER>(positive, str);
    }
    if (findComparat) {
        auto comparat = constructItem("Comparative", currentTab->inflectionForms);
        fillStructures<false, TYPE_GENDER>(comparat, str);
        fillStructures<false, TYPE_CASE>(comparat, str);
        fillStructures<false, TYPE_NUMBER>(comparat, str);
    }
    if (findSuperlat) {
        auto superlat = constructItem("Superlative", currentTab->inflectionForms);
        fillStructures<false, TYPE_DEFINITENESS>(superlat, str);
        fillStructures<false, TYPE_GENDER>(superlat, str);
        fillStructures<false, TYPE_CASE>(superlat, str);
        fillStructures<false, TYPE_NUMBER>(superlat, str);
    }
}

void MainWindow::fillPronouns(QString const & str) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());

    if (!currentTab->inflectionForms) return;

    fillStructures<true, TYPE_GENDER>(currentTab->inflectionForms, str);
    fillStructures<true, TYPE_CASE>(currentTab->inflectionForms, str);
    fillStructures<true, TYPE_NUMBER>(currentTab->inflectionForms, str);
}

void MainWindow::fillAdverbs(QString const & str) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());

    if (!currentTab->inflectionForms) return;

    bool findPositive = InflManager.find(str, Infl::Short, Infl::Positive);
    bool findComparat = InflManager.find(str, Infl::Short, Infl::Comparative);
    bool findSuperlat = InflManager.find(str, Infl::Short, Infl::Superlative);

    if (findPositive) constructItem(InflManager.nameOf(Infl::Positive), currentTab->inflectionForms);
    if (findComparat) constructItem(InflManager.nameOf(Infl::Comparative), currentTab->inflectionForms);
    if (findSuperlat) constructItem(InflManager.nameOf(Infl::Superlative), currentTab->inflectionForms);
}


QVector<QString> MainWindow::ParseCheckStateChangeInfo() {
    switch (currentPOS) {
    case (POS::Verb): {
        return ParseVerb();
    }
    case (POS::Noun): {
        return ParseNoun();
    }
    case (POS::Adjective): {
        return ParseAdjective();
    }
    case (POS::Adverb): {
        return ParseAdverb();
    }
    case (POS::Pronoun): {
        return ParsePronoun();
    }
    case (POS::Numerical): {
        return ParsePronoun();
    }
    }
}

QVector<QString> MainWindow::ParseVerb() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());

    QVector<QString> result;

    if (currentTab->inflStruct.empty()) return strvec_t{};

    auto end = currentTab->inflStruct.cend();
    auto itr = currentTab->inflStruct.cbegin();

    std::array<QVector<Infl::Forms>, 5> verbContainer;
    verbContainer.fill(QVector<Infl::Forms>{});

    while (itr != end && itr->at(0) != "Non-Impersonal") itr = std::next(itr);
    while (itr != end && itr->at(0) == "Non-Impersonal") {
        if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
            verbContainer.at(0).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Mood)) {
            verbContainer.at(1).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Tense)) {
            verbContainer.at(2).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Person)) {
            verbContainer.at(3).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
            verbContainer.at(4).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : verbContainer.at(0)) {
            for (auto && b : verbContainer.at(1)) {
                for (auto && c : verbContainer.at(2)) {
                    for (auto && d : verbContainer.at(3)) {
                        for (auto && e : verbContainer.at(4)) {
                            auto satisfied = !InflManager.find(entry, Infl::Short, Infl::Impersonal) &&
                                             !InflManager.find(entry, Infl::Short, Infl::Infinitive) &&
                                             !InflManager.find(entry, Infl::Short, Infl::Imperative) &&
                                             !InflManager.find(entry, Infl::Short, Infl::Supine) &&
                                             !InflManager.find(entry, Infl::Short, Infl::PresentParticiple) &&
                                             !InflManager.find(entry, Infl::Short, Infl::PastParticiple) &&
                                             InflManager.find(entry, Infl::Short, a) &&
                                             InflManager.find(entry, Infl::Short, b) &&
                                             InflManager.find(entry, Infl::Short, c) &&
                                             InflManager.find(entry, Infl::Short, d) &&
                                             InflManager.find(entry, Infl::Short, e);
                            if (satisfied) result.push_back(entry);
                        }
                    }
                }
            }
        }
    }

    std::array<QVector<Infl::Forms>, 5> impersonalContainer;
    impersonalContainer.fill(QVector<Infl::Forms>{});

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Impersonal)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Impersonal)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
            impersonalContainer.at(0).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Mood)) {
            impersonalContainer.at(1).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Tense)) {
            impersonalContainer.at(2).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Person)) {
            impersonalContainer.at(3).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
            impersonalContainer.at(4).push_back(InflManager.enumOfForms((itr->at(2))));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : impersonalContainer.at(0)) {
            for (auto && b : impersonalContainer.at(1)) {
                for (auto && c : impersonalContainer.at(2)) {
                    for (auto && d : impersonalContainer.at(3)) {
                        for (auto && e : impersonalContainer.at(4)) {
                            auto satisfied = InflManager.find(entry, Infl::Short, Infl::Impersonal) &&
                                             InflManager.find(entry, Infl::Short, a) &&
                                             InflManager.find(entry, Infl::Short, b) &&
                                             InflManager.find(entry, Infl::Short, c) &&
                                             InflManager.find(entry, Infl::Short, d) &&
                                             InflManager.find(entry, Infl::Short, e);
                            if (satisfied) result.push_back(entry);
                        }
                    }
                }
            }
        }
    }

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Infinitive)) itr = std::next(itr);

    QVector<Infl::Forms> infinitiveContainer;
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Infinitive)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
            infinitiveContainer.push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : infinitiveContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::Infinitive) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

    itr = currentTab->inflStruct.cbegin();
    QVector<Infl::Forms> imperativeContainer;
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Imperative)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Imperative)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
            imperativeContainer.push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : imperativeContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::Imperative) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Supine)) itr = std::next(itr);
    QVector<Infl::Forms> supineContainer;
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Supine)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
            supineContainer.push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : supineContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::Supine) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::PresentParticiple)) itr = std::next(itr);
    QVector<Infl::Forms> presPartContainer;

    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::PresentParticiple)) {
        presPartContainer.push_back(InflManager.enumOfForms(itr->at(0)));
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : presPartContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::PresentParticiple) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::PastParticiple)) itr = std::next(itr);
    std::array<QVector<Infl::Forms>, 4> participleContainter;
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::PastParticiple)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Definiteness)) {
            participleContainter.at(0).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Gender)) {
            participleContainter.at(1).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Case)) {
            participleContainter.at(2).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
            participleContainter.at(3).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : participleContainter.at(0)) {
            for (auto && b : participleContainter.at(1)) {
                for (auto && c : participleContainter.at(2)) {
                    for (auto && d : participleContainter.at(3)) {
                        auto satisfied = InflManager.find(entry, Infl::Short, Infl::Participle) &&
                                    InflManager.find(entry, Infl::Short, a) &&
                                    InflManager.find(entry, Infl::Short, b) &&
                                    InflManager.find(entry, Infl::Short, c) &&
                                    InflManager.find(entry, Infl::Short, d);
                        if (satisfied) result.push_back(entry);
                    }
                }
            }
        }
    }

    return result;
}

QVector<QString> MainWindow::ParseNoun() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QVector<QString> result;

    if (currentTab->inflStruct.empty()) return QVector<QString>{};

    auto itr = currentTab->inflStruct.cbegin();
    auto end = currentTab->inflStruct.cend();

    std::array<QVector<Infl::Forms>, 3> nounContainer;
    nounContainer.fill(QVector<Infl::Forms>{});

    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Definiteness)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Definiteness)) {
        nounContainer.at(0).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Case)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Case)) {
        nounContainer.at(1).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Number)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Number)) {
        nounContainer.at(2).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : nounContainer.at(0)) {
            for (auto && b : nounContainer.at(1)) {
                for (auto && c : nounContainer.at(2)) {
                    auto satisfied = InflManager.find(entry, Infl::Short, a) &&
                                     InflManager.find(entry, Infl::Short, b) &&
                                     InflManager.find(entry, Infl::Short, c);
                    if (satisfied) result.push_back(entry);
                }
            }
        }
    }

    return result;
}

QVector<QString> MainWindow::ParseAdjective() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QVector<QString> result;

    if (currentTab->inflStruct.empty()) return QVector<QString>{};

    auto itr = currentTab->inflStruct.cbegin();
    auto end = currentTab->inflStruct.cend();

    std::array<QVector<Infl::Forms>, 4> positiveContainer;
    positiveContainer.fill(QVector<Infl::Forms>{});

    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Positive)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Positive)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Definiteness)) {
            positiveContainer.at(0).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Gender)) {
            positiveContainer.at(1).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Case)) {
            positiveContainer.at(2).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
            positiveContainer.at(3).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : positiveContainer.at(0)) {
            for (auto && b : positiveContainer.at(1)) {
                for (auto && c : positiveContainer.at(2)) {
                    for (auto && d : positiveContainer.at(3)) {
                        auto satisfied = InflManager.find(entry, Infl::Short, Infl::Positive) &&
                                         InflManager.find(entry, Infl::Short, a) &&
                                         InflManager.find(entry, Infl::Short, b) &&
                                         InflManager.find(entry, Infl::Short, c) &&
                                         InflManager.find(entry, Infl::Short, d);
                        if (satisfied) result.push_back(entry);
                    }
                }
            }
        }
    }

    std::array<QVector<Infl::Forms>, 3> comparativeContainer;
    comparativeContainer.fill(QVector<Infl::Forms>{});

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Comparative)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Comparative)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Gender)) {
            comparativeContainer.at(0).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Case)) {
            comparativeContainer.at(1).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
            comparativeContainer.at(2).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : comparativeContainer.at(0)) {
            for (auto && b : comparativeContainer.at(1)) {
                for (auto && c : comparativeContainer.at(2)) {
                    auto satisfied = InflManager.find(entry, Infl::Short, Infl::Comparative) &&
                                     InflManager.find(entry, Infl::Short, a) &&
                                     InflManager.find(entry, Infl::Short, b) &&
                                     InflManager.find(entry, Infl::Short, c);
                    if (satisfied) result.push_back(entry);
                }
            }
        }
    }

    std::array<QVector<Infl::Forms>, 4> superlativeContainer;
    superlativeContainer.fill(QVector<Infl::Forms>{});

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Superlative)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Superlative)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Definiteness)) {
            superlativeContainer.at(0).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Gender)) {
            superlativeContainer.at(1).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Case)) {
            superlativeContainer.at(2).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        else if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
            superlativeContainer.at(3).push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : superlativeContainer.at(0)) {
            for (auto && b : superlativeContainer.at(1)) {
                for (auto && c : superlativeContainer.at(2)) {
                    for (auto && d : superlativeContainer.at(3)) {
                        auto satisfied = InflManager.find(entry, Infl::Short, Infl::Superlative) &&
                                         InflManager.find(entry, Infl::Short, a) &&
                                         InflManager.find(entry, Infl::Short, b) &&
                                         InflManager.find(entry, Infl::Short, c) &&
                                         InflManager.find(entry, Infl::Short, d);
                        if (satisfied) result.push_back(entry);
                    }
                }
            }
        }
    }
    return result;
}

QVector<QString> MainWindow::ParsePronoun() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QVector<QString> result;

    if (currentTab->inflStruct.empty()) return QVector<QString>{};

    std::array<QVector<Infl::Forms>, 3> pronounContainer;
    pronounContainer.fill(QVector<Infl::Forms>{});

    auto itr = currentTab->inflStruct.cbegin();
    auto end = currentTab->inflStruct.cend();

    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Gender)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Gender)) {
        pronounContainer.at(0).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Case)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Case)) {
        pronounContainer.at(1).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    itr = currentTab->inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Number)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Number)) {
        pronounContainer.at(2).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : pronounContainer.at(0)) {
            for (auto && b : pronounContainer.at(1)) {
                for (auto && c : pronounContainer.at(2)) {
                    auto satisfied = InflManager.find(entry, Infl::Short, a) &&
                                     InflManager.find(entry, Infl::Short, b) &&
                                     InflManager.find(entry, Infl::Short, c);
                    if (satisfied) result.push_back(entry);
                }
            }
        }
    }

    return result;
}

QVector<QString> MainWindow::ParseAdverb() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QVector<QString> result;

    if (currentTab->inflStruct.empty()) return QVector<QString>{};

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : currentTab->inflStruct) {
            if (a.at(0) == InflManager.nameOf(Infl::Positive)) {
                if (InflManager.find(entry, Infl::Short, Infl::Positive)) result.push_back(entry);
            }
            else if (a.at(0) == InflManager.nameOf(Infl::Comparative)) {
                if (InflManager.find(entry, Infl::Short, Infl::Comparative)) result.push_back(entry);

            }
            else if (a.at(0) == InflManager.nameOf(Infl::Superlative)) {
                if (InflManager.find(entry, Infl::Short, Infl::Superlative)) result.push_back(entry);
            }
        }
    }

    return result;
}

void MainWindow::onResultContextMenuRequested(QPoint const & p) {
    auto global = QWidget::mapToGlobal(p);
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto windowX = MainWindow::x();
    auto widgetX = currentTab->result->x();

    auto resultContextMenu = currentTab->result->createStandardContextMenu();
    resultContextMenu->move(windowX + widgetX + p.x(), global.y());
    resultContextMenu->addSeparator();

    QAction * act1 = new QAction("Search Icelandic -> English");
    QObject::connect(act1, &QAction::triggered,
                     this, &MainWindow::onContextMenuIceToEngTriggered);

    QAction * act2 = new QAction("Search Text In Icelandic Dictionary");
    QObject::connect(act2, &QAction::triggered,
                     this, &MainWindow::onContextMenuEngToIceTriggered);

    QAction * act3 = new QAction("Search Old Icelandic -> English");
    QObject::connect(act3, &QAction::triggered,
                     this, &MainWindow::onContextMenuNorToEngTriggered);

    QAction * act4 = new QAction("Search Text In Old Icelandic Dictionaries");
    QObject::connect(act4, &QAction::triggered,
                     this, &MainWindow::onContextMenuEngToNorTriggered);

    QAction * act5 = new QAction("Search Inflections Reversely");
    QObject::connect(act5, &QAction::triggered,
                     this, &MainWindow::onContextMenuSearchInfReverseTriggered);

    QAction * act6 = new QAction("Search Inflections");
    QObject::connect(act6, &QAction::triggered,
                     this, &MainWindow::onContextMenuSearchInfTriggered);

    /*
    QAction * zoomIn = new QAction("Zoom In");
    QObject::connect(zoomIn, &QAction::triggered,
                     this, &MainWindow::onContextMenuZoomInTriggered);
    zoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));

    QAction * zoomOut = new QAction("Zoom Out");
    QObject::connect(zoomOut, &QAction::triggered,
                     this, &MainWindow::onContextMenuZoomOutTriggered);
//    zoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_hyphen), this, SLOT(onContextMenuZoomOutTriggered()));
    */

    resultContextMenu->addAction(act1);
    resultContextMenu->addAction(act2);
    resultContextMenu->addAction(act3);
    resultContextMenu->addAction(act4);
    resultContextMenu->addAction(act5);
    resultContextMenu->addAction(act6);

    /*
    resultContextMenu->addSeparator();
    resultContextMenu->addAction(zoomIn);
    resultContextMenu->addAction(zoomOut);
    */
    resultContextMenu->show();
}

void MainWindow::onContextMenuIceToEngTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto cursor = currentTab->result->textCursor();
    auto text = cursor.selectedText();
    if (text.length() == 0) return;
    addTab_clicked();
    currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    on_actionModern_Icelandic_triggered();
    currentTab->input->setText(text);
    onInputEditingFinished();
}

void MainWindow::onContextMenuEngToIceTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto cursor = currentTab->result->textCursor();
    auto text = cursor.selectedText();
    if (text.length() == 0) return;
    addTab_clicked();
    currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    on_actionEnglish_Modern_Icelandic_triggered();
    currentTab->input->setText(text);
    onInputEditingFinished();
}

void MainWindow::onContextMenuNorToEngTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto cursor = currentTab->result->textCursor();
    auto text = cursor.selectedText();
    if (text.length() == 0) return;
    addTab_clicked();
    currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    on_actionOld_Icelandic_English_triggered();
    currentTab->input->setText(text);
    onInputEditingFinished();
}

void MainWindow::onContextMenuEngToNorTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto cursor = currentTab->result->textCursor();
    auto text = cursor.selectedText();
    if (text.length() == 0) return;
    addTab_clicked();
    currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    on_actionOld_Icelandic_Text_Search_triggered();
    currentTab->input->setText(text);
    onInputEditingFinished();
}

void MainWindow::onContextMenuSearchInfReverseTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto cursor = currentTab->result->textCursor();
    auto text = cursor.selectedText();
    if (text.length() == 0) return;
    addTab_clicked();
    currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    on_actionSearch_Inflections_triggered();
    currentTab->input->setText(text);
    onInputEditingFinished();
}

void MainWindow::onContextMenuSearchInfTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto cursor = currentTab->result->textCursor();
    auto text = cursor.selectedText();
    if (text.length() == 0) return;
    addTab_clicked();
    currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    on_actionList_All_Forms_triggered();
    currentTab->input->setText(text);
    onInputEditingFinished();
}

void MainWindow::onContextMenuZoomInTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto result = currentTab->result;
    auto text = result->toHtml();
    QString after;
    bool isPerpetua = text.contains(
                          QString("font-size:") +
                          to_string(currentTab->perpetuaFontSize).c_str() +
                          "px;");

    if (isPerpetua) {
        auto tokens = text.split(
                          QString("font-size:") +
                          to_string(currentTab->perpetuaFontSize).c_str() +
                          "px;");
        currentTab->perpetuaFontSize += 1;
        for (auto i = 0; i < tokens.size(); ++i) {
            if (i < tokens.size() - 1)
                after += tokens.at(i) + QString("font-size:") +
                         to_string(currentTab->perpetuaFontSize).c_str() + "px;";
            else
                after += tokens.at(i);
        }
    }
    else {
        auto tokens = text.split(QString("font-size:") +
                                 to_string(currentTab->segoeFontSize).c_str() +
                                 "px;");
        currentTab->segoeFontSize += 1;
        for (auto i = 0; i < tokens.size(); ++i) {
            if (i < tokens.size() - 1)
                after += tokens.at(i) + QString("font-size:") +
                         to_string(currentTab->segoeFontSize).c_str() + "px;";
            else
                after += tokens.at(i);
        }
    }

    currentTab->result->setHtml(after);
}

void MainWindow::onContextMenuZoomOutTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto result = currentTab->result;
    auto text = result->toHtml();
    QString after;

    bool isPerpetua = text.contains(
                          QString("font-size:") +
                          to_string(currentTab->perpetuaFontSize).c_str() + "px;");

    if (isPerpetua) {
        auto tokens = text.split(
                          QString("font-size:") +
                          to_string(currentTab->perpetuaFontSize).c_str() +
                          "px;");
        currentTab->perpetuaFontSize -= 1;
        for (auto i = 0; i < tokens.size(); ++i) {
            if (i < tokens.size() - 1)
                after += tokens.at(i) + QString("font-size:") +
                         to_string(currentTab->perpetuaFontSize).c_str() + "px;";
            else
                after += tokens.at(i);
        }
    }
    else {
        auto tokens = text.split(
                          QString("font-size:") +
                          to_string(currentTab->segoeFontSize).c_str() + "px;");
        currentTab->segoeFontSize -= 1;
        for (auto i = 0; i < tokens.size(); ++i) {
            if (i < tokens.size() - 1)
                after += tokens.at(i) + QString("font-size:") +
                         to_string(currentTab->segoeFontSize).c_str() + "px;";
            else
                after += tokens.at(i);
        }
    }

    currentTab->result->setHtml(after);
}


void MainWindow::on_actionZoom_In_triggered()
{
    onContextMenuZoomInTriggered();
}

void MainWindow::on_actionZoom_Out_triggered()
{
    onContextMenuZoomOutTriggered();
}
