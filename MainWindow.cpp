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
    ui->input->setPlaceholderText("Select a dictionary first...");
    ui->input->setFrame(false);
    ui->resultsTab->setDocumentMode(true);
    ui->resultsTab->setTabsClosable(true);
    ui->resultsTab->tabBar()->setMovable(true);
    ui->resultsTab->tabBar()->setAutoHide(true);
    ui->resultsTab->tabBar()->setExpanding(true);
    ui->statusBar->hide();
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
    result->setFrameStyle(QFrame::NoFrame);
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

void MainWindow::initializeResultFromDictionaries() {
    if (!resultsFromDictionaries) resultsFromDictionaries = new QListWidget;
    resultsFromDictionaries->setMaximumWidth(200);
    resultsFromDictionaries->setMaximumHeight(100);
    resultsFromDictionaries->setFrameStyle(QFrame::NoFrame);
    resultsFromDictionaries->setFocusPolicy(Qt::NoFocus);
    ui->inputLayout->addWidget(resultsFromDictionaries);
    QObject::connect(
                resultsFromDictionaries, &QListWidget::itemClicked,
                this, &MainWindow::resultsFromDictionaries_itemClicked
                );
}

void MainWindow::clearResultFromDictionaries() {
    if (resultsFromDictionaries) {
        resultsFromDictionaries->clear();
        delete resultsFromDictionaries;
        resultsFromDictionaries = nullptr;
    }
}

void MainWindow::initializeInflectionForms() {
    if (!inflectionForms) inflectionForms = new TreeWidget;
    inflectionForms->setHeaderLabel("Inflections");
    inflectionForms->setMaximumWidth(300);
    inflectionForms->setMinimumHeight(450);
    inflectionForms->setFrameStyle(QFrame::NoFrame);
    inflectionForms->setFocusPolicy(Qt::NoFocus);
    ui->inputLayout->addWidget(inflectionForms);
}

void MainWindow::clearInflectionForms() {
    if (inflectionForms) {
        inflectionForms->clear();
        delete inflectionForms;
        inflectionForms = nullptr;
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

    initializeInflectionForms();
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
    QString word = "<b>" + words + "</b>";
    auto upper = QString(words).toUpper();
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

    initializeResultFromDictionaries();

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
        QString key = i.first;
        key = key.mid(3, key.length() - 7);
//        key = key.substr(3, key.length() - 7);
        alternatives.push_back(key);
    }

    resultsFromDictionaries->clear();
    resultsFromDictionaries->addItems(alternatives);
    findDefinitionPrint(0);
}

void MainWindow::findDefinitionPrint(size_t index) {
    auto thisEntry = definitionResults.at(index);
    QString key = thisEntry.first;
    QString value;
    for (auto i : thisEntry.second) {
        value += i + '\n';
    }
    QString display = key + ' ' + value;
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 20pt;\">" + display + "</span></p>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(display);
}

void MainWindow::findInflection(QString const & word) {
    auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
    for (auto i = 0; i < 8; ++i) {
        results->push_back(strvecptr_t(new strvec_t));
    }

    for (size_t i = 0; i < 8; ++i) {
        findInflectionThread(results, word, i);
    }

    auto resultSize = [&]() { int sz = 0; for (auto i : *results) { sz += i->size(); } return sz; }();
    if (resultSize == 0) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setFontPointSize(20);
        tabActive->setText("Word not found");
        return;
    }
    QString toprint;
    for (auto && i : *results) {
        for (auto && j : *i) {
            j = addStyleToResults(j);
            toprint += j + "\n\n";
        }
    }
    toprint = "<span style=\"font-family: Segoe UI; font-size: 14pt;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint);
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
    auto thisEntry = textualResults.at(index);
    QString key = thisEntry.first;
    QString display = thisEntry.second;
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 20pt;\">" + display + "</span></p>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(display);
}

void MainWindow::textualSearchThread(QString word, size_t index) {
    auto thisDic = dictionaries->at(index);
    for (auto && i : *thisDic) {
        QString key = i.first;
        for (auto && j : *i.second) {
            /*
            auto pos = j.find(word);
            if (pos != std::string::npos) {
            */

            auto pos = j.indexOf(word);
            if (pos != -1) {
                auto subsitute = "<b><span style=\"color:#ff0000;\">" + word + "</span></b>";
                j.replace(pos, word.length(), subsitute);
                QString value = key + ' ' + j;
                textualResults.push_back(std::make_pair(key, value));
                break;
            }
        }
    }
}

void MainWindow::textualSearch(QString const & word) {
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
        QString key = i.first;
        if (key.length() > 7) {
            key = key.mid(3, key.length() - 7);
//            key = key.substr(3, key.length() - 7);
            alternatives.push_back(key);
        }
    }
    ui->options->addItems(alternatives);
}


void MainWindow::printAll(QString const & str) {
    ui->input->clear();
    ui->options->clear();
    auto word = str + ';';
    auto & results = resultsToPrint;
    for (size_t i = 0; i < 8; ++i) {
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
            ui->options->addItem(i.first);
    }
    printAllPrint(0);
}

void MainWindow::printAllThread(QString word, size_t index) {
    auto & thisDic = originals->at(index);
    auto & thisResult = resultsToPrint;
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
//                qDebug() << temp1.c_str() << temp2.c_str();
                if (currentPos == pos) { partOfSpeech = temp2; }
                if (temp1.c_str() != key) {
                    ++currentPos;
                    break;
                }
                else if (temp2 != partOfSpeech) {
//                    qDebug() << temp2.c_str();
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
    // thisResult is the vector that stores all the inflections of a given word
    auto thisResult = resultsToPrint[index];

    inflStruct.clear();
    inflSelectResult.clear();
    inflSelectResult = thisResult.second;

    QString toprint;

    for (auto i : thisResult.second) {
       QString temp = addStyleToResults(i);
       toprint += temp;
    }

    clearInflectionForms();
    initializeInflectionForms();
    fillInflectionForms(toprint);

    toprint = "<span style=\"font-family: Segoe UI; font-size: 14pt;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint);
}

void MainWindow::on_input_textEdited(const QString &arg1)
{
    QString word = arg1;
    ui->options->clear();
    if (flags[0] == 1) // search icelandic on
    {
        definitionResults.clear();
        QStringList display;
        if (word.length() > 1) {
            for (auto && entry : *wordindex) {
                auto pos = entry.indexOf(word);
//                auto pos = entry.find(word);
                if (pos == 0) {
                    display.push_back(entry);
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
}

void MainWindow::on_input_editingFinished()
{
    QString word = ui->input->text();
    if (flags[0] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        findDefinition(word);
    }
    else if (flags[1] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        ui->options->clear();
        textualSearch(word);
    }
    else if (flags[2] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        ui->options->clear();
        onlineEntries.clear();
        inputted = word;
        onlineDefinition(word);
    }
    else if (flags[3] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        findInflection(word);
    }
    else if (flags[4] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        ui->options->clear();
        resultsToPrint.clear();
        printAll(word);
    }
    else if (flags[6] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        ui->options->clear();
        onlineEntries.clear();
        inputted = word;
        onlineText(word);
    }
}


void MainWindow::on_options_itemClicked(QListWidgetItem *item)
{
    auto itemText = item->text();
    auto tag = itemText;

    if (flags[0] == 1) {
        ui->input->setText(itemText);
//        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        on_input_editingFinished();
    }

    else if (flags[1] == 1) {
        ui->input->setText(itemText);
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        if (textualResults.size() == 0) {
            textualSearch(tag);
        }
        else {
            size_t index = ui->options->currentRow();
            textualSearchPrint(index);
        }
    }

    else if (flags[2] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        QString url;
        auto itr = onlineEntries.find(tag);
        if (itr != onlineEntries.end()) {
            url = itr->second;
        }
        downloadPage(url);
    }
    else if (flags[4] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->clear();
        size_t index = ui->options->currentRow();
        printAllPrint(index);
// a function that prints according to the index.
    }
    else if (flags[6] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        QString url;
        auto itr = onlineEntries.find(tag);
        if (itr != onlineEntries.end()) {
            url = itr->second;
        }
        downloadPage(url);
    }
}


void MainWindow::resultsFromDictionaries_itemClicked(QListWidgetItem * item) {
    auto itemText = item->text();
    auto tag = itemText;
    if (flags[0] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);

        if (definitionResults.size() == 0) {
            findDefinition(tag);
        }
        else {
            size_t index = resultsFromDictionaries->currentRow();
            findDefinitionPrint(index);
        }
    }
}

void MainWindow::downloadPage(QString url) {
    ui->input->clear();
    QUrl pageUrl(url);
    pageControl = new PageDownloader(pageUrl, this);
    connect(pageControl, SIGNAL(downloaded()), this, SLOT(loadPage()));
}

void MainWindow::loadPage() {
    QByteArray qPage = (pageControl->downloadedData());
    QString str = QString::fromLatin1(qPage);
    webpage = str;
    if (parsePage()) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        webpage = "<span style=\"font-family: Perpetua; font-size: 20pt;\">" + webpage + "</span>";
        tabActive->setHtml(webpage);
        webpage.clear();
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

bool MainWindow::parsePage() {
    if (webpage.contains("produced no results.")) {
        auto pos = webpage.indexOf("<h3>While searching in Icelandic Online Dictionary and Readings</h3>");
        if (pos != -1) webpage = webpage.mid(pos, webpage.length() - pos);
        pos = webpage.contains("<div class=\"mainBackground\">");
        if (pos != -1) webpage = webpage.mid(0, pos);
        return true;
    }
    else if (webpage.contains("<div class=\"results\">")) {
        auto pos = webpage.indexOf("<div class=\"results\">");
        if (pos != -1) webpage = webpage.mid(pos, webpage.length() - pos);
        pos = webpage.indexOf("</div> <!-- results -->");
        if (pos != -1) webpage = webpage.mid(0, pos);
        QVector<QString> toBeDeleted = {
            "<div class=\"results\">", "<div class=\"nestlevel\">", "<span class=\"lemma\">",
            "<small><sup>", "</sup></small>", "</a></span>", "<span class=\"pos\">", "</span>",
            "</div>", "<!-- results -->", "<a href=\""
        };
        auto results = webpage;
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
//                pos = line.find("\">");
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
            onlineEntries.insert(std::make_pair(key.c_str(), link.c_str()));
        }

        QStringList alternatives;
        for (auto && i : onlineEntries) {
            alternatives.push_back(i.first);
        }
        ui->options->addItems(alternatives);
        return false;
    }
    else {
        auto pos = webpage.indexOf("<div class=\"entry\">");
        if (pos != -1) { webpage = webpage.mid(pos, webpage.length() - pos); }
        pos = webpage.indexOf("</div><!-- entry -->");
        if (pos != -1) { webpage = webpage.mid(0, pos); }
        pos = webpage.indexOf("<span class=\"lemma\">");
        if (pos != -1) {
            QString tag = "<span class=\"lemma\">";
            webpage.replace(pos, tag.length(), "<span style=\" font-weight:600; font-size:24pt;\" class=\"lemma\">");
        }
        while (webpage.contains("<span class=\"orth\">")) {
            QString tag = "<span class=\"orth\">";
            auto pos = webpage.indexOf(tag);
 //           auto pos = webpage.find(tag);
            if (pos != -1) {
                webpage.replace(pos, tag.length(), "<span style=\" font-weight:600;\" class=\"orth\">");
            }
        }
        while (webpage.contains("<span class=\"trans\">")) {
            QString tag = "<span class=\"trans\">";
            auto pos = webpage.indexOf(tag);
//            auto pos = webpage.find(tag);
            if (pos != -1) {
                webpage.replace(pos, tag.length(), "<span style=\" font-style:italic;\" class=\"trans\">");
            }
        }
        if (flags[6] == 1) {
            auto pos = webpage.indexOf(inputted);
            if (pos != -1) {
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
    clearInflectionForms();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_icelandic_word();
    ui->statusBar->showMessage("Search definitions for Modern Icelandic word (online)");
}


void MainWindow::on_actionEnglish_Modern_Icelandic_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_icelandic_text();
    ui->statusBar->showMessage("Search text in Modern Icelandic dictionary (online)");
}

void MainWindow::on_actionOld_Icelandic_English_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_norse_word();
    ui->statusBar->showMessage("Search definitions for Old Icelandic word");
}


void MainWindow::on_actionOld_Icelandic_Text_Search_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_norse_text();
    ui->statusBar->showMessage("Search text in Old Icelandic dictionaries");
}

void MainWindow::on_actionSearch_Inflections_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_original();
    ui->statusBar->showMessage("Search all inflection names of a word form");
}

void MainWindow::on_actionList_All_Forms_triggered()
{
    clearResultFromDictionaries();
    clearInflectionForms();
    ui->input->setEnabled(true);
    ui->options->setEnabled(true);
    search_all_inflections();
    ui->statusBar->showMessage("List all inflections of a word");
}


void MainWindow::checkStateChanged(Qt::CheckState state, QVector<QString> const vec) {
    if (state == Qt::CheckState::Checked) {
        if (inflStruct.find(vec) == inflStruct.end()) {
            inflStruct.insert(vec);
        }
    }
    else if (state == Qt::CheckState::Unchecked) {
        inflStruct.erase(inflStruct.find(vec));
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

    toprint = "<span style=\"font-family: Segoe UI; font-size: 14pt;\"><p align=\"center\"><table border=\"0.3\" cellpadding=\"10\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint);
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
    if (!inflectionForms) return;

    auto none = constructItem("Non-Impersonal", inflectionForms);
    fillStructures<false, TYPE_VOICE>(none, str);
    fillStructures<false, TYPE_MOOD>(none, str);
    fillStructures<false, TYPE_TENSE>(none, str);
    fillStructures<false, TYPE_PERSON>(none, str);
    fillStructures<false, TYPE_NUMBER>(none, str);

    bool findImpersonal = InflManager.find(str, Infl::Short, Infl::Impersonal);
    if (findImpersonal) {
        auto impersonal = constructItem(InflManager.nameOf(Infl::Impersonal), inflectionForms);
        fillStructures<false, TYPE_VOICE>(impersonal, str);
        fillStructures<false, TYPE_MOOD>(impersonal, str);
        fillStructures<false, TYPE_TENSE>(impersonal, str);
        fillStructures<false, TYPE_PERSON>(impersonal, str);
        fillStructures<false, TYPE_NUMBER>(impersonal, str);
    }

    bool findInfinitive = InflManager.find(str, Infl::Short, Infl::Infinitive);
    if (findInfinitive) {
        auto infinitive = constructItem(InflManager.nameOf(Infl::Infinitive), inflectionForms);
        fillStructures<false, TYPE_VOICE>(infinitive, str);
    }

    bool findImperative = InflManager.find(str, Infl::Short, Infl::Imperative);
    if (findImperative) {
        auto imperative = constructItem(InflManager.nameOf(Infl::Imperative), inflectionForms);
        fillStructures<false, TYPE_IMPERAT_NUMBER>(imperative, str);
    }

    bool findSupine = InflManager.find(str, Infl::Short, Infl::Supine);
    if (findSupine) {
        auto supine = constructItem(InflManager.nameOf(Infl::Supine), inflectionForms);
        fillStructures<false, TYPE_VOICE>(supine, str);
    }

    bool findPresParticiple = InflManager.find(str, Infl::Short, Infl::PresentParticiple);
    if (findPresParticiple) {
        auto participle = constructItem(InflManager.nameOf(Infl::PresentParticiple), inflectionForms);
    }

    bool findPastParticiple = InflManager.find(str, Infl::Short, Infl::PastParticiple);
    if (findPastParticiple) {
        auto participle = constructItem(InflManager.nameOf(Infl::PastParticiple), inflectionForms);
        fillStructures<false, TYPE_DEFINITENESS>(participle, str);
        fillStructures<false, TYPE_GENDER>(participle, str);
        fillStructures<false, TYPE_CASE>(participle, str);
        fillStructures<false, TYPE_NUMBER>(participle, str);
    }
}

void MainWindow::fillNouns(QString const & str) {
    if (!inflectionForms) return;
    fillStructures<true, TYPE_DEFINITENESS>(inflectionForms, str);
    fillStructures<true, TYPE_CASE>(inflectionForms, str);
    fillStructures<true, TYPE_NUMBER>(inflectionForms, str);
}

void MainWindow::fillAdjectives(QString const & str) {
    if (!inflectionForms) return;

    bool findPositive = InflManager.find(str, Infl::Short, Infl::Positive);
    bool findComparat = InflManager.find(str, Infl::Short, Infl::Comparative);
    bool findSuperlat = InflManager.find(str, Infl::Short, Infl::Superlative);

    if (!(findPositive || findComparat || findSuperlat)) return;

    if (findPositive) {
        auto positive = constructItem("Positive", inflectionForms);
        fillStructures<false, TYPE_DEFINITENESS>(positive, str);
        fillStructures<false, TYPE_GENDER>(positive, str);
        fillStructures<false, TYPE_CASE>(positive, str);
        fillStructures<false, TYPE_NUMBER>(positive, str);
    }
    if (findComparat) {
        auto comparat = constructItem("Comparative", inflectionForms);
        fillStructures<false, TYPE_GENDER>(comparat, str);
        fillStructures<false, TYPE_CASE>(comparat, str);
        fillStructures<false, TYPE_NUMBER>(comparat, str);
    }
    if (findSuperlat) {
        auto superlat = constructItem("Superlative", inflectionForms);
        fillStructures<false, TYPE_DEFINITENESS>(superlat, str);
        fillStructures<false, TYPE_GENDER>(superlat, str);
        fillStructures<false, TYPE_CASE>(superlat, str);
        fillStructures<false, TYPE_NUMBER>(superlat, str);
    }
}

void MainWindow::fillPronouns(QString const & str) {
    if (!inflectionForms) return;
    fillStructures<true, TYPE_GENDER>(inflectionForms, str);
    fillStructures<true, TYPE_CASE>(inflectionForms, str);
    fillStructures<true, TYPE_NUMBER>(inflectionForms, str);
}

void MainWindow::fillAdverbs(QString const & str) {
    if (!inflectionForms) return;

    bool findPositive = InflManager.find(str, Infl::Short, Infl::Positive);
    bool findComparat = InflManager.find(str, Infl::Short, Infl::Comparative);
    bool findSuperlat = InflManager.find(str, Infl::Short, Infl::Superlative);

    if (findPositive) constructItem(InflManager.nameOf(Infl::Positive), inflectionForms);
    if (findComparat) constructItem(InflManager.nameOf(Infl::Comparative), inflectionForms);
    if (findSuperlat) constructItem(InflManager.nameOf(Infl::Superlative), inflectionForms);
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
    QVector<QString> result;

    if (inflStruct.empty()) return strvec_t{};

    auto end = inflStruct.cend();
    auto itr = inflStruct.cbegin();

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

    for (auto && entry : inflSelectResult) {
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

    itr = inflStruct.cbegin();
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

    for (auto && entry : inflSelectResult) {
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

    itr = inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Infinitive)) itr = std::next(itr);

    QVector<Infl::Forms> infinitiveContainer;
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Infinitive)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
            infinitiveContainer.push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : inflSelectResult) {
        for (auto && a : infinitiveContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::Infinitive) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

    itr = inflStruct.cbegin();
    QVector<Infl::Forms> imperativeContainer;
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Imperative)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Imperative)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
            imperativeContainer.push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : inflSelectResult) {
        for (auto && a : imperativeContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::Imperative) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

    itr = inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Supine)) itr = std::next(itr);
    QVector<Infl::Forms> supineContainer;
    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::Supine)) {
        if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
            supineContainer.push_back(InflManager.enumOfForms(itr->at(2)));
        }
        itr = std::next(itr);
    }

    for (auto && entry : inflSelectResult) {
        for (auto && a : supineContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::Supine) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

    itr = inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::PresentParticiple)) itr = std::next(itr);
    QVector<Infl::Forms> presPartContainer;

    while (itr != end && itr->at(0) == InflManager.nameOf(Infl::PresentParticiple)) {
        presPartContainer.push_back(InflManager.enumOfForms(itr->at(0)));
        itr = std::next(itr);
    }

    for (auto && entry : inflSelectResult) {
        for (auto && a : presPartContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::PresentParticiple) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

    itr = inflStruct.cbegin();
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

    for (auto && entry : inflSelectResult) {
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
    QVector<QString> result;

    if (inflStruct.empty()) return QVector<QString>{};

    auto itr = inflStruct.cbegin();
    auto end = inflStruct.cend();

    std::array<QVector<Infl::Forms>, 3> nounContainer;
    nounContainer.fill(QVector<Infl::Forms>{});

    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Definiteness)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Definiteness)) {
        nounContainer.at(0).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    itr = inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Case)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Case)) {
        nounContainer.at(1).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    itr = inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Number)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Number)) {
        nounContainer.at(2).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    for (auto && entry : inflSelectResult) {
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
    QVector<QString> result;

    if (inflStruct.empty()) return QVector<QString>{};

    auto itr = inflStruct.cbegin();
    auto end = inflStruct.cend();

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

    for (auto && entry : inflSelectResult) {
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

    itr = inflStruct.cbegin();
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

    for (auto && entry : inflSelectResult) {
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

    itr = inflStruct.cbegin();
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

    for (auto && entry : inflSelectResult) {
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
    QVector<QString> result;

    if (inflStruct.empty()) return QVector<QString>{};

    std::array<QVector<Infl::Forms>, 3> pronounContainer;
    pronounContainer.fill(QVector<Infl::Forms>{});

    auto itr = inflStruct.cbegin();
    auto end = inflStruct.cend();

    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Gender)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Gender)) {
        pronounContainer.at(0).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    itr = inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Case)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Case)) {
        pronounContainer.at(1).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    itr = inflStruct.cbegin();
    while (itr != end && itr->at(0) != InflManager.categoryOf(Infl::Number)) itr = std::next(itr);
    while (itr != end && itr->at(0) == InflManager.categoryOf(Infl::Number)) {
        pronounContainer.at(2).push_back(InflManager.enumOfForms(itr->at(1)));
        itr = std::next(itr);
    }

    for (auto && entry : inflSelectResult) {
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
    QVector<QString> result;

    if (inflStruct.empty()) return QVector<QString>{};

    for (auto && entry : inflSelectResult) {
        for (auto && a : inflStruct) {
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

