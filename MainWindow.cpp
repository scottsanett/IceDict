#include "QObject"
#include "MainWindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    Q_INIT_RESOURCE(resource);
    ui->setupUi(this);
    this->setWindowTitle("IceDict");
    ui->actionClose_Tab->setText("Close Window");
    ui->actionUndo_Close_Tab->setEnabled(false);
    ui->actionBack->setEnabled(false);
    ui->actionForward->setEnabled(false);
    ui->actionFind_in_Page->setEnabled(false);
    ui->resultsTab->setDocumentMode(true);
    ui->resultsTab->setTabsClosable(true);
    ui->resultsTab->tabBar()->setMovable(true);
    ui->resultsTab->tabBar()->setAutoHide(true);
    ui->resultsTab->tabBar()->setExpanding(true);

    appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(appDataLocation)) dir.mkpath(appDataLocation);

    pageControl = new PageDownloader(this);
    QObject::connect(pageControl, SIGNAL(downloaded()), this, SLOT(loadPage()));
    QObject::connect(pageControl, SIGNAL(connectionError()), this, SLOT(connectionError()));
    QObject::connect(pageControl, SIGNAL(timeoutError()), this, SLOT(timeoutError()));

    QObject::connect(ui->resultsTab->tabBar(), &QTabBar::tabCloseRequested,
                     this, &MainWindow::onTabCloseButtonClicked);
    statusBar = new StatusBar(this);
    this->setStatusBar(statusBar);
    statusBar->hide();
    QObject::connect(ui->resultsTab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    addTab_clicked();

    checkDatabaseIntegrity();
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::importBINDBs() {
    updateDialog->deleteLater();
    importWordIndex();
    importDictionary();
    importOriginal();
    importInflections();
}

void MainWindow::addTab_clicked() {
    auto currentTab = std::make_shared<Pimpl>();
    currentTab->centralLayout = new QVBoxLayout();
    currentTab->mainSplitter = new QSplitter();
#ifdef _WIN32
    currentTab->mainSplitter->setHandleWidth(1);
#else
    currentTab->mainSplitter->setHandleWidth(1);
#endif
    tabIndices.insert(std::make_pair(currentTab->mainSplitter, currentTab));
    tabResultHistory.insert(std::make_pair(currentTab->mainSplitter, std::deque<QString>{}));
    tabResultHistoryIndex.insert(std::make_pair(currentTab->mainSplitter, -1));
    currentTab->centralLayout->addWidget(currentTab->mainSplitter);
    currentTab->inputLayout = new QSplitter();
#ifdef _WIN32
    currentTab->inputLayout->setHandleWidth(1);
#else
    currentTab->inputLayout->setHandleWidth(1);
#endif
    currentTab->inputLayout->setContentsMargins(0, 0, 0, 0);
    currentTab->inputLayout->setFrameStyle(QFrame::NoFrame);
    currentTab->inputLayout->setOrientation(Qt::Vertical);
    currentTab->mainSplitter->addWidget(currentTab->inputLayout);

    currentTab->inputPaneLayout = new QVBoxLayout;
    currentTab->inputPaneLayout->setSpacing(0);
    currentTab->inputPaneLayout->setMargin(0);
    currentTab->inputPaneLayoutWidget = new QWidget;
    currentTab->inputPaneLayoutWidget->setLayout(currentTab->inputPaneLayout);
    currentTab->inputLayout->addWidget(currentTab->inputPaneLayoutWidget);

    currentTab->buttonLayout = new QHBoxLayout;
    currentTab->buttonLayout->setSpacing(0);
#ifdef _WIN32
    currentTab->buttonLayout->setMargin(0);
#elif __APPLE__
    currentTab->buttonLayout->setMargin(0);
#endif
    currentTab->buttonLayoutWidget = new QWidget();
    currentTab->buttonLayoutWidget->setLayout(currentTab->buttonLayout);
#ifdef _WIN32
    currentTab->backButton = new QPushButton("Back");
#elif __APPLE__
    currentTab->backButton = new QPushButton("⬅");
#endif
    currentTab->backButton->setStyleSheet("font-size: 13px");
    currentTab->backButton->setEnabled(false);
    auto buttonSizeHint = currentTab->backButton->sizeHint();
#ifdef _WIN32
    currentTab->buttonLayoutWidget->setMinimumWidth(buttonSizeHint.width() * 2);
    currentTab->buttonLayoutWidget->setMaximumWidth(buttonSizeHint.width() * 3);
#elif __APPLE__
    currentTab->buttonLayoutWidget->setMinimumWidth(buttonSizeHint.width() * 3);
    currentTab->buttonLayoutWidget->setMaximumWidth(buttonSizeHint.width() * 4);
#endif
    QObject::connect(currentTab->backButton, &QPushButton::pressed,
                     this, &MainWindow::on_actionBack_triggered);
#ifdef _WIN32
    currentTab->nextButton = new QPushButton("Next");
#elif __APPLE__
    currentTab->nextButton = new QPushButton("➡︎");
#endif
    currentTab->nextButton->setStyleSheet("font-size: 13px");
    currentTab->nextButton->setEnabled(false);
    QObject::connect(currentTab->nextButton, &QPushButton::pressed,
                     this, &MainWindow::on_actionForward_triggered);

    currentTab->buttonLayout->addWidget(currentTab->backButton);
    currentTab->buttonLayout->addWidget(currentTab->nextButton);
    currentTab->inputPaneLayout->addWidget(currentTab->buttonLayoutWidget);

    currentTab->comboBox = new QComboBox();
#ifdef _WIN32
    currentTab->comboBox->addItem("Icelandic to English");
    currentTab->comboBox->addItem("Icelandic Textual");
    currentTab->comboBox->addItem("Norse to English");
#elif __APPLE__
    currentTab->comboBox->addItem("Icelandic → English");
    currentTab->comboBox->addItem("Icelandic Textual");
    currentTab->comboBox->addItem("Norse → English");
#endif
    currentTab->comboBox->addItem("Norse Textual");
    currentTab->comboBox->addItem("Find Headword");
    currentTab->comboBox->addItem("All Inflections");
    currentTab->comboBox->setStyleSheet("font-size: 13px");
    currentTab->comboBox->setCurrentIndex(-1);
    connect(currentTab->comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onComboBoxIndexChanged(int)));
    currentTab->inputPaneLayout->addWidget(currentTab->comboBox);
    auto comboBoxSizeHint = currentTab->comboBox->sizeHint();

    currentTab->input = new QLineEdit();
    currentTab->input->setPlaceholderText("Select a dictionary first...");
    currentTab->input->setStyleSheet("font-size: 13px");
    currentTab->input->setClearButtonEnabled(true);
    currentTab->input->setEnabled(false);
    currentTab->inputPaneLayout->addWidget(currentTab->input, 1, 0);
    QObject::connect(currentTab->input, &QLineEdit::textEdited,
                     this, &MainWindow::onInputTextEdited);
    QObject::connect(currentTab->input, &QLineEdit::returnPressed,
                     this, &MainWindow::onInputReturnPressed);
    auto inputBoxSizeHint = currentTab->input->sizeHint();
#ifdef _WIN32
    currentTab->inputPaneLayoutWidget->setMaximumHeight(buttonSizeHint.height() + comboBoxSizeHint.height() + inputBoxSizeHint.height());
#elif __APPLE__
    currentTab->inputPaneLayoutWidget->setMaximumHeight(buttonSizeHint.height() + comboBoxSizeHint.height() + inputBoxSizeHint.height() + 10);
#endif

    currentTab->options = new QListWidget(this);
#ifdef _WIN32
    currentTab->options->setMinimumWidth(buttonSizeHint.width() * 2);
    currentTab->options->setMaximumWidth(buttonSizeHint.width() * 3);
#elif __APPLE__
    currentTab->options->setMinimumWidth(buttonSizeHint.width() * 3);
    currentTab->options->setMaximumWidth(buttonSizeHint.width() * 4);
#endif
    currentTab->options->setStyleSheet("font-size: 14px");
    currentTab->options->setFrameStyle(QFrame::HLine);
    currentTab->options->setEnabled(false);
    currentTab->inputLayout->addWidget(currentTab->options);
    QObject::connect(currentTab->options, &QListWidget::itemClicked,
                     this, &MainWindow::onOptionsItemClicked);

    currentTab->resultLayout = new QSplitter;
    currentTab->resultLayout->setOrientation(Qt::Vertical);
    currentTab->resultLayout->setFrameStyle(QFrame::NoFrame);
    currentTab->resultLayout->setHandleWidth(1);
    currentTab->resultLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    currentTab->mainSplitter->addWidget(currentTab->resultLayout);
    currentTab->result = new QTextBrowser();
    currentTab->result->setHtml(startScreen);
    currentTab->result->setFrameStyle(QFrame::NoFrame);
    currentTab->result->setFont(QFont("Georgia"));
    currentTab->result->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(currentTab->result, &QTextBrowser::anchorClicked,
                     this, &MainWindow::onResultUrlClicked);
    currentTab->resultLayout->addWidget(currentTab->result);

    auto index = ui->resultsTab->currentIndex();
    ui->resultsTab->insertTab(++index, currentTab->mainSplitter, ("empty"));
    QObject::connect(currentTab->result, &QTextBrowser::customContextMenuRequested,
                     this, &MainWindow::onResultContextMenuRequested);
    ui->resultsTab->setCurrentIndex(index);
    if (ui->resultsTab->count() > 1) {
        ui->actionClose_Tab->setText("Close Tab");
    }
}

void MainWindow::closeTab(int index) {
    auto currentSplitter = ui->resultsTab->widget(index);
    if (tabIndices.find(currentSplitter) != tabIndices.end()) {
        if (closedTabs.size() == 10) {
            closedTabs.pop_front();
        }
        closedTabs.push_back(std::make_tuple(index, ui->resultsTab->tabText(index), tabIndices.at(currentSplitter)));
        tabIndices.erase(currentSplitter);
        ui->resultsTab->removeTab(index);
    }
    ui->actionUndo_Close_Tab->setEnabled(true);
}

void MainWindow::searchPanelReturnPressed(QString str, QTextDocument::FindFlags flags) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto document = currentTab->result->document();
    auto text_format = QTextCharFormat();
    auto palette = currentTab->result->palette();
    text_format.setBackground(QBrush(Qt::yellow));

    findInPageSelections.clear();
    findInPageSelectionIndex = 0;
    QTextCursor cursor;

    while (true) {
        cursor = document->find(str, cursor, flags);
        if (cursor.isNull()) break;
        QTextEdit::ExtraSelection sel;
        sel.cursor = cursor;
        sel.format = text_format;
        findInPageSelections.append(sel);
    }

    currentTab->result->setExtraSelections(findInPageSelections);
}

void MainWindow::searchPaneNextButtonPressed() {
    if (findInPageSelections.empty()) return;
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (findInPageSelectionIndex < findInPageSelections.size()) {
        if (++findInPageSelectionIndex == findInPageSelections.size()) {
            findInPageSelectionIndex = findInPageSelections.size() - 1;
        }
        auto sel = findInPageSelections.at(findInPageSelectionIndex);
        currentTab->result->setTextCursor(sel.cursor);
    }
}

void MainWindow::searchPanePrevButtonPressed() {
    if (findInPageSelections.empty()) return;
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (findInPageSelectionIndex >= 0) {
        if (--findInPageSelectionIndex < 0) findInPageSelectionIndex = 0;
        auto sel = findInPageSelections.at(findInPageSelectionIndex);
        currentTab->result->setTextCursor(sel.cursor);
    }
}

void MainWindow::activateInput() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->activateWindow();
    currentTab->input->setFocus();
    currentTab->input->selectAll();
}

void MainWindow::initializeInflectionForms() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms)
        currentTab->inflectionForms = new TreeWidget(this);
    currentTab->inflectionForms->setHeaderLabel("Inflections");
    auto buttonSizeHint = currentTab->backButton->sizeHint();
#ifdef _WIN32
    currentTab->inflectionForms->setMinimumWidth(buttonSizeHint.width() * 2);
    currentTab->inflectionForms->setMaximumWidth(buttonSizeHint.width() * 3);
#elif __APPLE__
    currentTab->inflectionForms->setMinimumWidth(buttonSizeHint.width() * 3);
    currentTab->inflectionForms->setMaximumWidth(buttonSizeHint.width() * 4);
#endif
    currentTab->inflectionForms->setFrameStyle(QFrame::NoFrame);
    currentTab->inflectionForms->setStyleSheet("font-size: 14px");
    currentTab->inputLayout->addWidget(currentTab->inflectionForms);

    if (!currentTab->proceedButton) {
        currentTab->proceedButton = new QPushButton("Proceed", this);
        QObject::connect(currentTab->proceedButton, &QPushButton::pressed,
                         this, &MainWindow::proceedButtonPressed);
        currentTab->proceedButton->setFixedHeight(30);
        currentTab->proceedButton->setStyleSheet("font-size: 13px");
        currentTab->inputLayout->addWidget(currentTab->proceedButton);
    }
}

void MainWindow::clearInflectionForms() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (currentTab->inflectionForms) {
        currentTab->inflectionForms->clear();
        delete currentTab->inflectionForms;
        currentTab->inflectionForms = nullptr;
    }
    if (currentTab->proceedButton) {
        delete currentTab->proceedButton;
        currentTab->proceedButton = nullptr;
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
    std::string arg2, arg3;
    iss >> arg2 >> arg3;
    arg3 = arg3.substr(0, arg3.length() - 1);
    arg3 = "<td>" + arg3 + "</td>";
    std::string arg4;
    iss >> arg4;
    arg4 = arg4.substr(0, arg4.length() - 1);
    arg4 = "<td><font color=\"red\">" + arg4 + "</font></td>";
    std::string arg5;
    std::string temp;
    while (iss >> temp) {
        arg5 += temp + ' ';
    }
    arg5 = arg5.substr(0, arg5.length() - 1);
    arg5 = "<td>" + arg5 + "</td>";
    result = "<tr>" + key + arg3 + arg4 + arg5 + "</tr>";
    return result.c_str();
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

QString MainWindow::oldToModern(QString word, Infl::Transforms flag) {
    if (flag & Infl::Vowels) {
        auto oeIndex = word.indexOf("œ");
        if (oeIndex != -1) word.replace(oeIndex, 1, "æ");
        auto oslashIndex = word.indexOf("ø");
        if (oslashIndex != -1) word.replace(oslashIndex, 1, "ö");
        if (word.endsWith("ékk")) {
            word.replace(word.length() - 3, 1, "e");
        }
        while (word.indexOf("áng") != -1) {
            word.replace(word.indexOf("áng"), 1, "a");
        }
        while (word.indexOf("éng") != -1) {
            word.replace(word.indexOf("éng"), 1, "e");
        }
        while (word.indexOf("íng") != -1) {
            word.replace(word.indexOf("íng"), 1, "i");
        }
        while (word.indexOf("óng") != -1) {
            word.replace(word.indexOf("óng"), 1, "o");
        }
        while (word.indexOf("úng") != -1) {
            word.replace(word.indexOf("úng"), 1, "u");
        }
        while (word.indexOf("ýng") != -1) {
            word.replace(word.indexOf("ýng"), 1, "y");
        }
    }

    if (flag & Infl::Consonants) {
        while (word.contains("pt")) {
            auto index = word.indexOf("pt");
            word.replace(index, 1, "f");
        }
        if (word.endsWith("rr")) {
            word = word.left(word.length() - 1);
        }
        else if (word.endsWith("r") && !isVowel(word.at(word.length() - 2))) {
            word.insert(word.length() - 1, "u");
        }
        else if (word.endsWith("ss")) {
            word = word.left(word.length() - 1);
        }
        while (word.contains("z")) {
            auto index = word.indexOf("z");
            word.replace(index, 1, "s");
        }
    }

    if (flag & Infl::Inflections) {
        if (word.endsWith("it")) {
            word.replace(word.length() - 1, 1, "ð");
        }
        else if (word.endsWith("at")) {
            word.replace(word.length() - 1, 1, "ð");
        }
        else if (word.endsWith("ðisk")) {
            word.replace(word.length() - 4, 1, "t");
        }
        else if (word.endsWith("ðir")) {
            word.replace(word.length() - 3, 1, "t");
        }
        else if (word.endsWith("ði")) {
            word.replace(word.length() - 2, 1, "t");
        }
        else if (word.endsWith("isk")) {
            word.replace(word.length() - 1, 1, "t");
        }
        else if (word.endsWith("ask")) {
            word.replace(word.length() - 1, 1, "t");
        }
        else if (word == "emk") {
            word = "er";
        }
        else if (word.endsWith("mk")) {
            word = word.left(word.length() - 1);
        }
        else if (word == "ek" || word == "eg") {
            word = "ég";
        }
        /*
        else if (word.endsWith("a")) {
            word.replace(word.length() - 1, 1, "i");
        }
        */
    }
    return word;
}


bool MainWindow::isVowel(QChar ch) {
    return (std::find(vowels.cbegin(), vowels.cend(), ch) != vowels.cend());
}

/* import the index for all words in the two dictionaries */
void MainWindow::importWordIndex() {
    QFile wd(":/alphabet/wordindex");
    wd.open(QIODevice::ReadOnly);
    QString file = wd.readAll();
    wordIndexList = file.split("\n");
    wordIndexList.sort(Qt::CaseInsensitive);
    wd.close();
    norseWordCompleter = new QCompleter(wordIndexList, this);
    norseWordCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    norseWordCompleter->setMaxVisibleItems(30);
}

void MainWindow::importInflections() {
    std::thread t1(&MainWindow::importInflectionsThread, this, std::ref(inflectionals), 1);
    std::thread t2(&MainWindow::importInflectionsThread, this, std::ref(inflectionals), 2);
    std::thread t3(&MainWindow::importInflectionsThread, this, std::ref(inflectionals), 3);
    std::thread t4(&MainWindow::importInflectionsThread, this, std::ref(inflectionals), 4);
    t1.join(); t2.join(); t3.join(); t4.join();

    std::thread t5(&MainWindow::importInflectionsThread, this, std::ref(inflectionals), 5);
    std::thread t6(&MainWindow::importInflectionsThread, this, std::ref(inflectionals), 6);
    std::thread t7(&MainWindow::importInflectionsThread, this, std::ref(inflectionals), 7);
    std::thread t8(&MainWindow::importInflectionsThread, this, std::ref(inflectionals), 8);
    t5.join(); t6.join(); t7.join(); t8.join();
}


/*import all the inflection forms and its position*/
void MainWindow::importInflectionsThread(std::array<map_t, 8> & mapvec, size_t i) {
    QString filename = appDataLocation + "/db3/part" + QString(to_string(i).c_str());
    QFile f(filename);

    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    int index = 0;
    auto && thisMap = mapvec[i - 1];
    while (std::getline(file, line)) {
        thisMap.insert(std::make_pair(line.c_str(), index));
        ++index;
    }
    f.close();
}

void MainWindow::importOriginal() {
    std::thread t1(&MainWindow::importOriginalThread, this, std::ref(originals), 1);
    std::thread t2(&MainWindow::importOriginalThread, this, std::ref(originals), 2);
    std::thread t3(&MainWindow::importOriginalThread, this, std::ref(originals), 3);
    std::thread t4(&MainWindow::importOriginalThread, this, std::ref(originals), 4);
    t1.join(); t2.join(); t3.join(); t4.join();

    std::thread t5(&MainWindow::importOriginalThread, this, std::ref(originals), 5);
    std::thread t6(&MainWindow::importOriginalThread, this, std::ref(originals), 6);
    std::thread t7(&MainWindow::importOriginalThread, this, std::ref(originals), 7);
    std::thread t8(&MainWindow::importOriginalThread, this, std::ref(originals), 8);
    t5.join(); t6.join(); t7.join(); t8.join();
}

void MainWindow::importOriginalThread(std::array<map_t, 8> & mapvec, size_t i) {
    QString filename = QString(appDataLocation + "/db2/part") + to_string(i).c_str();
    QFile f(filename);

    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    auto && thisMap = mapvec[i - 1];
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
        thisMap.insert(std::make_pair(key.c_str(), index_number));
    }

    f.close();
}

void MainWindow::importDictionary() {
    std::thread t1(&MainWindow::importDictionaryThread, this, "zoega", 0);
    std::thread t2(&MainWindow::importDictionaryThread, this, "vifgusson", 1);
    t1.join(); t2.join();
}

void MainWindow::importDictionaryThread(QString const name, size_t i) {
    QString filename = ":/alphabet/" + name;
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QString qfile = f.readAll();
    std::istringstream file(qfile.toStdString());
    std::string line;
    auto && thisMap = dictionaries[i];
    while (std::getline(file, line)) {
        auto entry = vecstr_t{};
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        std::string sense;
        std::string temp;
        while (iss >> temp) {
            sense += ' ' + temp;
            if (temp.back() == ';') {
               sense += '\n';
               entry.push_back(sense.c_str());
               sense = "";
            }
        }
        thisMap.insert(std::make_pair(key.c_str(), entry));
    }
    f.close();
}

void MainWindow::findDefinition(QString word) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->definitionResults.clear();
    auto original = word;

    auto upper = QString(original).toUpper();
    word = "<b>" + original + "</b>";
    upper = "<b>" + upper + "</b>";

    auto IsAllUpper = [](QString const & str){
        for (auto && i : str) {
            if (!i.isUpper()) return false;
        }
        return true;
    };

    auto zisspair = dictionaries.at(0).equal_range(word);
    auto visspair = dictionaries.at(1).equal_range(word);
    auto uisspair = dictionaries.at(1).equal_range(upper);

    if ((zisspair.first == zisspair.second) && (visspair.first == visspair.second) && (uisspair.first == uisspair.second))
    {
//        currentTab->result->setFontFamily("Perpetua");
        currentTab->result->setFontPointSize(20);
        currentTab->result->setText("Word not found");
        return;
    }

    currentTab->definitionResults.clear();
    for (auto itr = zisspair.first; itr != zisspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = itr->second;
        currentTab->definitionResults.insert(std::make_pair(key, thisEntry));
    }
    for (auto itr = visspair.first; itr != visspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = itr->second;
        currentTab->definitionResults.insert(std::make_pair(key, thisEntry));
    }

    if (!IsAllUpper(original)) {
        for (auto itr = uisspair.first; itr != uisspair.second; ++itr) {
            auto key = itr->first;
            auto thisEntry = itr->second;
            currentTab->definitionResults.insert(std::make_pair(key, thisEntry));
        }
    }

    QStringList alternatives;
    for (auto && i : currentTab->definitionResults) {
        QString key = i.first;
        key = key.mid(3, key.length() - 7);
        alternatives.push_back(key.toLower());
    }

    currentTab->options->clear();
    currentTab->options->addItems(alternatives);
    findDefinitionPrint(0);
}

void MainWindow::findDefinitionPrint(size_t index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto thisEntry = currentTab->definitionResults.cbegin();
    for (auto i = 0; i < index; ++i) thisEntry = std::next(thisEntry);

    QString key = thisEntry->first;
    QString value;
    for (auto i : thisEntry->second) {
        value += i + '\n';
    }
    QString display = key + ' ' + value;
    display = "<span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px;\">" + display + "</span></p>";
    currentTab->result->setHtml(display);
    onResultTextChanged(display);
}

void MainWindow::findInflection(QString word) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    std::array<vecstr_t, 8> results;
    results.fill(vecstr_t{});

    if ([](QString str){ for (auto && i : str) { if (i.isLower()) return false; } return true; }(word)) {
        word = word.toLower();
    }

    word = oldToModern(word, Infl::Vowels | Infl::Consonants);
    auto wordInfl = oldToModern(word, Infl::Inflections);

    for (size_t i = 0; i < 8; ++i) {
        findInflectionThread(results, word, i);
    }


    if (word != wordInfl) {
        for (size_t i = 0; i < 8; ++i) {
            findInflectionThread(results, wordInfl, i);
        }
    }

    auto resultSize = [&]() { int sz = 0; for (auto i : results) { sz += i.size(); } return sz; }();
    if (resultSize == 0) {
        currentTab->result->setFontPointSize(20);
        currentTab->result->setText("Word not found");
        return;
    }

    QString toprint;
    for (auto && i : results) {
        for (auto && j : i) {
            j = addStyleToResults(j);
            toprint += j + "\n\n";
        }
    }

    toprint = "<span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px;\"><p align=\"center\"><table border=\"1\" cellpadding=\"10\">" + toprint + "</table></p></span>";
    currentTab->result->setHtml(toprint);
    onResultTextChanged(toprint);
}

void MainWindow::findInflectionThread(std::array<vecstr_t, 8> & results, QString word, size_t index) {
    auto && thisDic = inflectionals[index];
    auto && thisResult = results[index];
    auto itr = thisDic.find(word);
    if (itr == thisDic.end()) { return; }
    QString filename = QString(appDataLocation + "/db1/part") + to_string(index + 1).c_str();
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
            thisResult.push_back(line.c_str());
            itr = std::next(itr);
            ++currentPos;
            pos = itr->second;
        }
    }

    file.close();
}

void MainWindow::textualSearchPrint(size_t index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto thisEntry = currentTab->textualResults.cbegin();
    for (auto i = 0; i < index; ++i) thisEntry = std::next(thisEntry);
    QString key = thisEntry->first;
    QString display = thisEntry->second;
    display = "<span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px;\">" + display + "</span></p>";
    currentTab->result->setHtml(display);
    onResultTextChanged(display);
}

void MainWindow::textualSearchThread(QString word, size_t index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto thisDic = dictionaries.at(index);
    for (auto && i : thisDic) {
        QString key = i.first;
        QString value;
        bool found = false;
        for (auto j : i.second) {
            auto pos = j.indexOf(word);
            if (pos != -1) {
                found = true;
                auto subsitute = "<b><span style=\"color:#ff0000;\">" + word + "</span></b>";
                j.replace(pos, word.length(), subsitute);
            }
            value += ' ' + j;
        }
        value = key + ' ' + value;
        if (found) currentTab->textualResults.insert(std::make_pair(key, value));
    }
}

void MainWindow::textualSearch(QString const & word) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->textualResults.clear();

    textualSearchThread(word, 0);
    textualSearchThread(word, 1);

    if (currentTab->textualResults.size() == 0) {
//        currentTab->result->setFontFamily("Perpetua");
        currentTab->result->setFontPointSize(20);
        currentTab->result->setText("Word not found.");
        return;
    }

    QStringList alternatives;
    for (auto i : currentTab->textualResults) {
        QString key = i.first;
        if (key.length() > 7) {
            key = key.mid(3, key.length() - 7);
            key = key.toLower();
            alternatives.push_back(key);
        }
    }

    currentTab->options->addItems(alternatives);
}


void MainWindow::printAll(QString word) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->clear();
    currentTab->options->clear();

    if ([](QString str){ for (auto && i : str) { if (i.isLower()) return false; } return true; }(word)) {
        word = word.toLower();
    }
    word = oldToModern(word, Infl::Vowels | Infl::Consonants);
    word = word + ';';

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
    auto & thisDic = originals.at(index);
    auto & thisResult = currentTab->resultsToPrint;
    auto range = thisDic.equal_range(word);
    auto count = std::distance(range.first, range.second);
    if (count == 0)  return;
    QString filename = QString(appDataLocation + "/db1/part") + to_string(index + 1).c_str();
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line, wordIndex;
    int currentPos = 0;
    for (auto itr = range.first; itr != range.second; ++itr) {
        auto key = itr->first;
        vecstr_t thisEntry;
        auto pos = itr->second;

        while (std::getline(issfile, line)) {
            if (currentPos < pos) { ++currentPos; continue; }
            else {
                std::istringstream iss(line);
                std::string temp1;
                iss >> temp1;
                std::string temp2;
                iss >> temp2;
                if (currentPos == pos) { wordIndex = temp2; }
                if (temp1.c_str() != key) { ++currentPos; break;}
                else if (temp2 != wordIndex) {
                    ++currentPos;
                    wordIndex = temp2;
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

    clearInflectionForms();
    initializeInflectionForms();
    fillInflectionForms(thisResult.second);

    QString display;
    for (auto i : thisResult.second) {
       QString temp = addStyleToResults(i);
       display += temp;
    }

    display = "<span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px;\"><p align=\"center\"><table border=\"1\" cellpadding=\"10\">" + display + "</table></p></span>";
    currentTab->result->setHtml(display);
    onResultTextChanged(display);
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
    }
    else if (currentTab->flags[3] == 1) {
        currentTab->textualResults.clear();
    }
    else if (currentTab->flags[4] == 1) {
    }
    else if (currentTab->flags[5] == 1) {
        currentTab->resultsToPrint.clear();
    }
}


void MainWindow::onInputReturnPressed()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QString word = currentTab->input->text();

    if (currentTab->flags[0] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        currentTab->options->clear();
        currentTab->onlineEntries.clear();
        currentTab->textInQuery.clear();
        onlineDefinition(word);
    }
    else if (currentTab->flags[1] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        currentTab->options->clear();
        currentTab->onlineEntries.clear();
        currentTab->textInQuery.clear();
        onlineText(word);
    }
    else if (currentTab->flags[2] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        findDefinition(word);
    }
    else if (currentTab->flags[3] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        textualSearch(word);
    }
    else if (currentTab->flags[4] == 1 && word.length() > 0) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), word);
        currentTab->options->clear();
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
    auto itemIndex = currentTab->options->currentRow();
    auto itemText = item->text();
    auto tag = itemText;

    if (currentTab->flags[0] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        QString url;
        auto itr = currentTab->onlineEntries.cbegin();
        for (auto i = 0; i < itemIndex; ++i) itr = std::next(itr);
        downloadPage(itr->second);
    }
    else if (currentTab->flags[1] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        QString url;
        auto itr = currentTab->onlineEntries.cbegin();
        for (auto i = 0; i < itemIndex; ++i) itr = std::next(itr);
        downloadPage(itr->second);
    }
    else if (currentTab->flags[2] == 1) {
        /*
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        findDefinition(tag);
        */
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        if (currentTab->definitionResults.size() == 0) {
            findDefinition(tag);
        }
        else {
            auto index = currentTab->options->currentRow();
            findDefinitionPrint(index);
        }
    }
    else if (currentTab->flags[3] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        if (currentTab->textualResults.size() == 0) {
            textualSearch(tag);
        }
        else {
            auto index = currentTab->options->currentRow();
            textualSearchPrint(index);
        }
    }
    else if (currentTab->flags[5] == 1) {
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        currentTab->result->clear();
        auto index = currentTab->options->currentRow();
        printAllPrint(index);
    }
}

/*
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
*/

void MainWindow::connectionError() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->result->setHtml("<br/><br/><br/><br/><br/><span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px; font-weight: bold;\"><p align='center'>Connection Error</p><p align='center' style='font-weight: normal'>Please check your network connection.</p></span>");
}

void MainWindow::timeoutError() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->result->setHtml("<br/><br/><br/><br/><b/><span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px; font-weight: bold;\"><p align='center'>Connection Timeout</p><p align='center' style='font-weight: normal'>Please check your network connection.</p></span>");
}

void MainWindow::onlineText(QString word) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    word = word.toLower();
    word = oldToModern(word, Infl::Vowels);
    currentTab->textInQuery = word;
    QString url = QString(textUrl1).toUtf8() + word + QString(textUrl2).toUtf8();
    downloadPage(url);
}

void MainWindow::onlineDefinition(QString word) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    word = word.toLower();
    word = oldToModern(word, Infl::Vowels | Infl::Consonants);
    currentTab->textInQuery = word;
    QString url = QString(searchUrl1).toUtf8() + word + QString(searchUrl2).toUtf8();
    downloadPage(url);
}

QString MainWindow::removeNonAlpha(QString str) {
    QString result;
    for (auto ch : str) {
        if (ch.isLetter()) result += ch;
    }
    return result;
}

void MainWindow::downloadPage(QString url) {
    pageControl->DownloadPage(QUrl{url});
}

void MainWindow::loadPage() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QByteArray qPage = (pageControl->downloadedData());
    QString str = QString::fromUtf8(qPage);

    currentTab->webpage = str;

    auto display = parsePage();
    if (display == Infl::Results::No || display == Infl::Results::One) {
        currentTab->webpage = "<span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px;\">" + currentTab->webpage + "</span>";
        currentTab->result->setHtml(currentTab->webpage);
        onResultTextChanged(currentTab->webpage);
        currentTab->webpage.clear();
    }
    else if (display == Infl::Results::Many) {
        currentTab->options->clear();
        for (auto && i : currentTab->onlineEntries) {
            currentTab->options->addItem(i.first);
        }
        auto firstEntry = currentTab->onlineEntries.cbegin();
        auto tag = firstEntry->first;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag);
        downloadPage(firstEntry->second);
    }
    else if (display == Infl::Results::Maybe) {
        currentTab->webpage = "<span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px;\">" + currentTab->webpage + "</span>";
        currentTab->result->setHtml(currentTab->webpage);
        onResultTextChanged(currentTab->webpage);
        currentTab->webpage.clear();
        onlineText(currentTab->wordAfterRedirection);
        currentTab->wordAfterRedirection.clear();
    }
}

Infl::Results MainWindow::parsePage() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
//    qDebug() << currentTab->webpage;
    QString kwStartMarker = "Icelandic Online: Dictionary Entry for ";
    QString kwEndMarker = "</title>\n<link rel=\"stylesheet\"";
    auto kwStartIndex = currentTab->webpage.indexOf(kwStartMarker);
    auto kwEndIndex = currentTab->webpage.indexOf(kwEndMarker);
    auto kwRaw = currentTab->webpage.mid(kwStartIndex + kwStartMarker.length(), kwEndIndex - kwStartIndex - kwStartMarker.length());
    QString keyword;
    for (auto ch : kwRaw) {
        if (ch.isLetter()) keyword += ch;
    }
    if (currentTab->webpage.contains("produced no results."))  // no results
    {
        if (currentTab->flags.at(1) == 1) {
            QString startMarker = "simple search for <em>";
            QString endMarker = "</em> produced no results";
            auto startPos = currentTab->webpage.indexOf(startMarker);
            auto endPos = currentTab->webpage.indexOf(endMarker);
            auto word = currentTab->webpage.mid(startPos + startMarker.length(), endPos - startPos - startMarker.length());
            auto backup = word;
            word = oldToModern(word, Infl::Consonants | Infl::Inflections);
            if (word == backup) {
                currentTab->webpage = "Your simple search for <em>" + backup + "</em> produced no results.</p>\n\n<p>Modify your search and try again:</p>\n\n\t";
                return Infl::Results::No;
//                return true;
            }
            currentTab->webpage = "No result has been found.\n\nRedirecting to " + word + "...";
            currentTab->wordAfterRedirection = word;
            return Infl::Results::Maybe;
//            return false;
        }
        QString startMarker = "<h3>While searching in Icelandic Online Dictionary and Readings</h3>";
        QString endMarker = ":</p>\n\n\t<div class=\"mainBackground\">\n\t\t";
        auto startPos = currentTab->webpage.indexOf(startMarker);
        auto endPos = currentTab->webpage.indexOf(endMarker);
        currentTab->webpage = currentTab->webpage.mid(startPos + startMarker.length(), endPos - startPos - startMarker.length()) + ".";
        return Infl::Results::No;
    }

    else if (currentTab->webpage.contains("<div class=\"results\">")) // found multiples results
    {
        auto pos = currentTab->webpage.indexOf("<div class=\"results\">");
        if (pos != -1)
            currentTab->webpage = currentTab->webpage.mid(pos, currentTab->webpage.length() - pos);
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
            QString key_str = key.c_str();
            if (key_str.begin()->isDigit()) key_str = key_str.right(key_str.length() - 1);
            currentTab->onlineEntries.insert(std::make_pair(key_str, link.c_str()));
        }
        return Infl::Results::Many;
    }

    else // found only one result
    {
        QString startMarker = "<div class=\"entry\">";
        QString endMarker = "</div><!-- entry -->";
        auto startPos = currentTab->webpage.indexOf(startMarker);
        auto endPos = currentTab->webpage.indexOf(endMarker);
        currentTab->webpage = currentTab->webpage.mid(startPos + startMarker.length(), endPos - startPos - startMarker.length());
        auto pos = currentTab->webpage.indexOf("<span class=\"lemma\">");
        if (pos != -1) {
            QString tag = "<span class=\"lemma\">";
            currentTab->webpage.replace(pos, tag.length(), "<span style=\" font-weight: bold; font-size: " + QString::fromStdString(std::to_string(currentTab->fontSize)) + "px;\" class=\"lemma\">");
        }
        while (currentTab->webpage.contains("<span class=\"orth\">")) {
            QString tag = "<span class=\"orth\">";
            auto pos = currentTab->webpage.indexOf(tag);
            if (pos != -1) {
                currentTab->webpage.replace(pos, tag.length(), "<span style=\" font-weight: bold;\" class=\"orth\">");
            }
        }
        while (currentTab->webpage.contains("<span class=\"trans\">")) {
            QString tag = "<span class=\"trans\">";
            auto pos = currentTab->webpage.indexOf(tag);
            if (pos != -1) {
                currentTab->webpage.replace(pos, tag.length(), "<span style=\" font-style:italic;\" class=\"trans\">");
            }
        }
        return Infl::Results::One;
    }
}

void MainWindow::onResultUrlClicked(QUrl url) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    QString finalUrl = "http://digicoll.library.wisc.edu" + url.toString();
    downloadPage(finalUrl);
}

void MainWindow::onResultTextChanged(QString display) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (display == "<span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px;\"><p align=\"center\"><table border=\"1\" cellpadding=\"10\"></table></p></span>") return;

    if (tabResultHistoryIndex.at(currentTab->mainSplitter) < tabResultHistory.at(currentTab->mainSplitter).size() - 1) {
        auto historyIndex = tabResultHistoryIndex.at(currentTab->mainSplitter);
        auto& historyContainer = tabResultHistory.at(currentTab->mainSplitter);
        auto historySize = historyContainer.size();

        for (auto i = 0 ; i < historySize - 1 - historyIndex; ++i) {
            historyContainer.pop_back();
        }
    }

    tabResultHistory.at(currentTab->mainSplitter).push_back(display);
    tabResultHistoryIndex.at(currentTab->mainSplitter) += 1;

    if (tabResultHistory.at(currentTab->mainSplitter).size() > 1) {
        ui->actionBack->setEnabled(true);
        ui->actionForward->setEnabled(false);
        currentTab->backButton->setEnabled(true);
        currentTab->nextButton->setEnabled(false);
    }
    else {
        ui->actionBack->setEnabled(false);
        ui->actionForward->setEnabled(false);
        currentTab->backButton->setEnabled(false);
        currentTab->nextButton->setEnabled(false);
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
        ui->actionFullscreen->setText("Exit Full Screen");
    }
    else {
        QWidget::setWindowState(Qt::WindowNoState);
        ui->actionFullscreen->setText("Enter Full Screen");
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
    else {
        this->close();
    }
}

void MainWindow::onComboBoxIndexChanged(int index) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->input->setEnabled(true);
    currentTab->options->setEnabled(true);
    clearInflectionForms();
    switch (index) {
    case (0): { on_actionModern_Icelandic_triggered(); break; }
    case (1): { on_actionEnglish_Modern_Icelandic_triggered(); break; }
    case (2): { on_actionOld_Icelandic_English_triggered(); break; }
    case (3): { on_actionOld_Icelandic_Text_Search_triggered(); break; }
    case (4): { on_actionSearch_Inflections_triggered(); break; }
    case (5): { on_actionList_All_Forms_triggered(); break; }
    default: {}
    }
}

void MainWindow::onTabCloseButtonClicked(int index) {
    if (ui->resultsTab->count() > 1) {
        closeTab(index);
    }
    else {
        ui->actionClose_Tab->setText("Close Window");
    }
}

void MainWindow::on_actionModern_Icelandic_triggered()
{
    clearInflectionForms();

    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    ui->actionFind_in_Page->setEnabled(true);

    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->comboBox->setCurrentIndex(0);
    currentTab->flags = {{1, 0, 0, 0, 0, 0}};
    currentTab->input->clear();
    currentTab->input->setCompleter(nullptr);
    currentTab->onlineEntries.clear();
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert word here...");
    statusBar->showMessage("search definitions for modern Icelandic word (online)");
}


void MainWindow::on_actionEnglish_Modern_Icelandic_triggered()
{
    clearInflectionForms();

    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    ui->actionFind_in_Page->setEnabled(true);

    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->comboBox->setCurrentIndex(1);
    currentTab->flags = {{0, 1, 0, 0, 0, 0}};
    currentTab->input->clear();
    currentTab->input->setCompleter(nullptr);
    currentTab->onlineEntries.clear();
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert text here...");
    statusBar->showMessage("search text in modern Icelandic dictionary (online)");
}

void MainWindow::on_actionOld_Icelandic_English_triggered()
{
    clearInflectionForms();
    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    ui->actionFind_in_Page->setEnabled(true);

    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->comboBox->setCurrentIndex(2);
    currentTab->flags = {{0, 0, 1, 0, 0, 0}};
    currentTab->input->setPlaceholderText(tr("Insert word here..."));
    currentTab->input->clear();
    currentTab->input->setCompleter(norseWordCompleter);
    currentTab->result->clear();
    currentTab->options->clear();
    statusBar->showMessage("search definitions for old Icelandic word");
}


void MainWindow::on_actionOld_Icelandic_Text_Search_triggered()
{
    clearInflectionForms();
    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    ui->actionFind_in_Page->setEnabled(true);
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->comboBox->setCurrentIndex(3);
    currentTab->flags = {{0, 0, 0, 1, 0, 0}};
    currentTab->input->clear();
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert text here...");
    statusBar->showMessage("search text in old Icelandic dictionaries");
}

void MainWindow::on_actionSearch_Inflections_triggered()
{
    clearInflectionForms();
    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    ui->actionFind_in_Page->setEnabled(true);
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->comboBox->setCurrentIndex(4);
    currentTab->flags = {{0, 0, 0, 0, 1, 0}};
    currentTab->input->clear();
    currentTab->input->setCompleter(nullptr);
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert word here...");
    statusBar->showMessage("find all entries that can be inflected to the word just entered");
}

void MainWindow::on_actionList_All_Forms_triggered()
{
    clearInflectionForms();
    ui->actionZoom_In->setEnabled(true);
    ui->actionZoom_Out->setEnabled(true);
    ui->actionFind_in_Page->setEnabled(true);
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    currentTab->comboBox->setCurrentIndex(5);
    currentTab->flags = {{0, 0, 0, 0, 0, 1}};
    currentTab->input->clear();
    currentTab->input->setCompleter(nullptr);
    currentTab->options->clear();
    currentTab->result->clear();
    currentTab->input->setPlaceholderText("Insert word here...");
    statusBar->showMessage("search all inflections of a word");
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
}

void MainWindow::proceedButtonPressed() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto resultVec = ParseCheckStateChangeInfo();
    QString toprint;
    for (auto i : resultVec) {
       QString temp = addStyleToResults(i);
       toprint += temp;
    }
    toprint = "<span style=\"font-size: " + QString::fromStdString(to_string(currentTab->fontSize)) + "px;\"><p align=\"center\"><table border=\"1\" cellpadding=\"10\">" + toprint + "</table></p></span>";
    currentTab->result->setHtml(toprint);
    onResultTextChanged(toprint);
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

void MainWindow::fillInflectionForms(vecstr_t const & result) {
    // rewrite this function except for verbs
    auto && str = result.at(0);
    if (str.contains(PartOfSpeech.MasNoun)) {
        currentPOS = POS::Noun;
        fillNouns(result);
    }
    else if (str.contains(PartOfSpeech.FemNoun)) {
        currentPOS = POS::Noun;
        fillNouns(result);
    }
    else if (str.contains(PartOfSpeech.NetNoun)) {
        currentPOS = POS::Noun;
        fillNouns(result);
    }
    else if (str.contains(PartOfSpeech.Verb)) {
        currentPOS = POS::Verb;
        fillVerbs(result);
    }
    else if (str.contains(PartOfSpeech.Adjective)) {
        currentPOS = POS::Adjective;
        fillAdjectives(result);
    }
    else if (str.contains(PartOfSpeech.Adverb)) {
        currentPOS = POS::Adverb;
        fillAdverbs(result);
    }
    else if (str.contains(PartOfSpeech.Pronoun)) {
        currentPOS = POS::Pronoun;
        fillPronouns(result);
    }
    else if (str.contains(PartOfSpeech.Numerical)) {
        currentPOS = POS::Numerical;
        fillPronouns(result);
    }
    else ;
}

void MainWindow::fillVerbs(vecstr_t const & result) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms) return;

    QString none_str,
            impersonal_str,
            question_str,
            infinitive_str,
            imperative_str,
            supine_str,
            presparticiple_str,
            pastparticiple_str;

    for (auto && item : result) {
        if (InflManager.find(item, Infl::Short, Infl::Impersonal)) {
            impersonal_str += item + ";";
        }
        else if (InflManager.find(item, Infl::Short, Infl::Question)) {
            question_str += item + ";";
        }
        else if (InflManager.find(item, Infl::Short, Infl::Infinitive)) {
            infinitive_str += item + ";";
        }
        else if (InflManager.find(item, Infl::Short, Infl::Imperative)) {
            imperative_str += item + ";";
        }
        else if (InflManager.find(item, Infl::Short, Infl::Supine)) {
            supine_str += item + ";";
        }
        else if (InflManager.find(item, Infl::Short, Infl::PresentParticiple)) {
            presparticiple_str += item + ";";
        }
        else if (InflManager.find(item, Infl::Short, Infl::PastParticiple)) {
            pastparticiple_str += item + ";";
        }
        else {
            none_str += item + ";";
        }
    }

    if (!none_str.isEmpty()) {
        auto none = constructItem("Non-Impersonal", currentTab->inflectionForms);
        fillStructures<false, TYPE_VOICE>(none, none_str);
        fillStructures<false, TYPE_MOOD>(none, none_str);
        fillStructures<false, TYPE_TENSE>(none, none_str);
        fillStructures<false, TYPE_PERSON>(none, none_str);
        fillStructures<false, TYPE_NUMBER>(none, none_str);
    }

    if (!impersonal_str.isEmpty()) {
        auto impersonal = constructItem(InflManager.nameOf(Infl::Impersonal), currentTab->inflectionForms);
        fillStructures<false, TYPE_SUBJECT_CASE>(impersonal, impersonal_str);
        fillStructures<false, TYPE_VOICE>(impersonal, impersonal_str);
        fillStructures<false, TYPE_MOOD>(impersonal, impersonal_str);
        fillStructures<false, TYPE_TENSE>(impersonal, impersonal_str);
        fillStructures<false, TYPE_PERSON>(impersonal, impersonal_str);
        fillStructures<false, TYPE_NUMBER>(impersonal, impersonal_str);
    }

    if (!question_str.isEmpty()) {
        auto question = constructItem(InflManager.nameOf(Infl::Question), currentTab->inflectionForms);
        fillStructures<false, TYPE_VOICE>(question, question_str);
        fillStructures<false, TYPE_MOOD>(question, question_str);
        fillStructures<false, TYPE_TENSE>(question, question_str);
        fillStructures<false, TYPE_PERSON>(question, question_str);
        fillStructures<false, TYPE_NUMBER>(question, question_str);
    }

    if (!infinitive_str.isEmpty()) {
        auto infinitive = constructItem(InflManager.nameOf(Infl::Infinitive), currentTab->inflectionForms);
        fillStructures<false, TYPE_VOICE>(infinitive, infinitive_str);
    }

    if (!imperative_str.isEmpty()) {
        auto imperative = constructItem(InflManager.nameOf(Infl::Imperative), currentTab->inflectionForms);
        fillStructures<false, TYPE_IMPERAT_NUMBER>(imperative, imperative_str);
    }

    if (!supine_str.isEmpty()) {
        auto supine = constructItem(InflManager.nameOf(Infl::Supine), currentTab->inflectionForms);
        fillStructures<false, TYPE_VOICE>(supine, supine_str);
    }

    if (!presparticiple_str.isEmpty() || !pastparticiple_str.isEmpty()) {
        auto participle = constructItem(InflManager.nameOf(Infl::Participle), currentTab->inflectionForms);
        if (!presparticiple_str.isEmpty())
            constructItem(InflManager.nameOf(Infl::PresentParticiple), participle);
        if (!pastparticiple_str.isEmpty()) {
            auto pastParticiple = constructItem(InflManager.nameOf(Infl::PastParticiple), participle);
            fillStructures<false, TYPE_DEFINITENESS>(pastParticiple, pastparticiple_str);
            fillStructures<false, TYPE_GENDER>(pastParticiple, pastparticiple_str);
            fillStructures<false, TYPE_CASE>(pastParticiple, pastparticiple_str);
            fillStructures<false, TYPE_NUMBER>(pastParticiple, pastparticiple_str);
        }
    }

//TODO: past participle needs to go through the same process
}

void MainWindow::fillNouns(vecstr_t const & result) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms) return;

    QString str;

    for (auto && i : result) {
        str += i + ";";
    }

    fillStructures<true, TYPE_DEFINITENESS>(currentTab->inflectionForms, str);
    fillStructures<true, TYPE_CASE>(currentTab->inflectionForms, str);
    fillStructures<true, TYPE_NUMBER>(currentTab->inflectionForms, str);
}

void MainWindow::fillAdjectives(vecstr_t const & result) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms) return;

    QString str;

    for (auto && i : result) {
        str += i + ";";
    }

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

void MainWindow::fillPronouns(vecstr_t const & result) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms) return;

    QString str;

    for (auto && i : result) {
        str += i + ";";
    }

    fillStructures<true, TYPE_GENDER>(currentTab->inflectionForms, str);
    fillStructures<true, TYPE_CASE>(currentTab->inflectionForms, str);
    fillStructures<true, TYPE_NUMBER>(currentTab->inflectionForms, str);
}

void MainWindow::fillAdverbs(vecstr_t const & result) {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (!currentTab->inflectionForms) return;

    QString str;

    for (auto && i : result) {
        str += i + ";";
    }

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

    if (currentTab->inflStruct.empty()) return vecstr_t{};

    auto end = currentTab->inflStruct.cend();
    auto itr = currentTab->inflStruct.cbegin();

        /********** Non-Impersonal  *********/
    std::array<QVector<Infl::Forms>, 5> verbContainer;
    verbContainer.fill(QVector<Infl::Forms>{});

    while (itr != end) {
        if (itr->at(0) == "Non-Impersonal") {
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
                                             !InflManager.find(entry, Infl::Short, Infl::Question) &&
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

        /********** Impersonal *********/
    std::array<QVector<Infl::Forms>, 6> impersonalContainer;
    impersonalContainer.fill(QVector<Infl::Forms>{});

    itr = currentTab->inflStruct.cbegin();
//    while (itr != end && itr->at(0) != InflManager.nameOf(Infl::Impersonal)) itr = std::next(itr);
    while (itr != end) {
        if (itr->at(0) == InflManager.nameOf(Infl::Impersonal)) {
            if (itr->at(1) == InflManager.categoryOf(Infl::SubjectCase)) {
                impersonalContainer.at(0).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
                impersonalContainer.at(1).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Mood)) {
                impersonalContainer.at(2).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Tense)) {
                impersonalContainer.at(3).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Person)) {
                impersonalContainer.at(4).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
                impersonalContainer.at(5).push_back(InflManager.enumOfForms((itr->at(2))));
            }
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : impersonalContainer.at(0)) {
            for (auto && b : impersonalContainer.at(1)) {
                for (auto && c : impersonalContainer.at(2)) {
                    for (auto && d : impersonalContainer.at(3)) {
                        for (auto && e : impersonalContainer.at(4)) {
                            for (auto && f : impersonalContainer.at(5)) {
                                    auto satisfied =
                                        InflManager.find(entry, Infl::Short, Infl::Impersonal) &&
                                             InflManager.find(entry, Infl::Short, a) &&
                                             InflManager.find(entry, Infl::Short, b) &&
                                             InflManager.find(entry, Infl::Short, c) &&
                                             InflManager.find(entry, Infl::Short, d) &&
                                             InflManager.find(entry, Infl::Short, e) &&
                                             InflManager.find(entry, Infl::Short, f);
                                if (satisfied) result.push_back(entry);
                            }
                        }
                    }
                }
            }
        }
    }

        /********** Question *********/
    std::array<QVector<Infl::Forms>, 5> questionContainer;
    questionContainer.fill(QVector<Infl::Forms>{});
    itr = currentTab->inflStruct.cbegin();
    while (itr != end) {
        if (itr->at(0) == InflManager.nameOf(Infl::Question)) {
            if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
                questionContainer.at(0).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Mood)) {
                questionContainer.at(1).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Tense)) {
                questionContainer.at(2).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Person)) {
                questionContainer.at(3).push_back(InflManager.enumOfForms((itr->at(2))));
            }
            else if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
                questionContainer.at(4).push_back(InflManager.enumOfForms((itr->at(2))));
            }
        }
        itr = std::next(itr);
    }

    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : questionContainer.at(0)) {
            for (auto && b : questionContainer.at(1)) {
                for (auto && c : questionContainer.at(2)) {
                    for (auto && d : questionContainer.at(3)) {
                        for (auto && e : questionContainer.at(4)) {
                            auto satisfied = InflManager.find(entry, Infl::Short, Infl::Question) &&
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


        /********** Infinitive *********/
    itr = currentTab->inflStruct.cbegin();
    QVector<Infl::Forms> infinitiveContainer;
    while (itr != end) {
        if (itr->at(0) == InflManager.nameOf(Infl::Infinitive)) {
            if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
                infinitiveContainer.push_back(InflManager.enumOfForms(itr->at(2)));
            }
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

            /********** Imperative *********/
    itr = currentTab->inflStruct.cbegin();
    QVector<Infl::Forms> imperativeContainer;
    while (itr != end) {
        if (itr->at(0) == InflManager.nameOf(Infl::Imperative)) {
            if (itr->at(1) == InflManager.categoryOf(Infl::Number)) {
                imperativeContainer.push_back(InflManager.enumOfForms(itr->at(2)));
            }
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

            /********** Supine *********/
    itr = currentTab->inflStruct.cbegin();
    QVector<Infl::Forms> supineContainer;
    while (itr != end) {
        if (itr->at(0) == InflManager.nameOf(Infl::Supine)) {
            if (itr->at(1) == InflManager.categoryOf(Infl::Voice)) {
                supineContainer.push_back(InflManager.enumOfForms(itr->at(2)));
            }
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

            /********** Participle *********/
    itr = currentTab->inflStruct.cbegin();
    QVector<Infl::Forms> presPartContainer;
    std::array<QVector<Infl::Forms>, 4> pastPartContainer;
    while (itr != end) {
        if (itr->at(0) == InflManager.nameOf(Infl::Participle)) {
            if (itr->at(1) == InflManager.nameOf(Infl::PresentParticiple)) {
                presPartContainer.push_back(InflManager.enumOfForms(itr->at(1)));
            }
            else if (itr->at(1) == InflManager.nameOf(Infl::PastParticiple)) {
                if (itr->at(2) == InflManager.categoryOf(Infl::Definiteness)) {
                    pastPartContainer.at(0).push_back(InflManager.enumOfForms(itr->at(3)));
                }
                else if (itr->at(2) == InflManager.categoryOf(Infl::Gender)) {
                    pastPartContainer.at(1).push_back(InflManager.enumOfForms(itr->at(3)));
                }
                else if (itr->at(2) == InflManager.categoryOf(Infl::Case)) {
                    pastPartContainer.at(2).push_back(InflManager.enumOfForms(itr->at(3)));
                }
                else if (itr->at(2) == InflManager.categoryOf(Infl::Number)) {
                    pastPartContainer.at(3).push_back(InflManager.enumOfForms(itr->at(3)));
                }
            }
        }
        itr = std::next(itr);
    }

            /********** Present Participle *********/
    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : presPartContainer) {
            auto satisfied = InflManager.find(entry, Infl::Short, Infl::PresentParticiple) &&
                             InflManager.find(entry, Infl::Short, a);
            if (satisfied) result.push_back(entry);
        }
    }

            /********** Past Participle *********/
    for (auto && entry : currentTab->inflSelectResult) {
        for (auto && a : pastPartContainer.at(0)) {
            for (auto && b : pastPartContainer.at(1)) {
                for (auto && c : pastPartContainer.at(2)) {
                    for (auto && d : pastPartContainer.at(3)) {
                        auto satisfied = InflManager.find(entry, Infl::Short, Infl::PastParticiple) &&
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

void MainWindow::onResultContextMenuRequested(QPoint const &) {
    auto cursorPos = QCursor::pos();
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto resultContextMenu = currentTab->result->createStandardContextMenu();
    resultContextMenu->move(cursorPos.x(), cursorPos.y());
    resultContextMenu->addSeparator();

    QAction * act1 = new QAction("Icelandic → English", this);
    QObject::connect(act1, &QAction::triggered,
                     this, &MainWindow::onContextMenuIceToEngTriggered);

    QAction * act2 = new QAction("Icelandic Textual", this);
    QObject::connect(act2, &QAction::triggered,
                     this, &MainWindow::onContextMenuEngToIceTriggered);

    QAction * act3 = new QAction("Norse → English", this);
    QObject::connect(act3, &QAction::triggered,
                     this, &MainWindow::onContextMenuNorToEngTriggered);

    QAction * act4 = new QAction("Norse Textual", this);
    QObject::connect(act4, &QAction::triggered,
                     this, &MainWindow::onContextMenuEngToNorTriggered);

    QAction * act5 = new QAction("Find Headword", this);
    QObject::connect(act5, &QAction::triggered,
                     this, &MainWindow::onContextMenuSearchInfReverseTriggered);

    QAction * act6 = new QAction("All Inflections", this);
    QObject::connect(act6, &QAction::triggered,
                     this, &MainWindow::onContextMenuSearchInfTriggered);

    resultContextMenu->addAction(act1);
    resultContextMenu->addAction(act2);
    resultContextMenu->addAction(act3);
    resultContextMenu->addAction(act4);
    resultContextMenu->addAction(act5);
    resultContextMenu->addAction(act6);
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
    onInputReturnPressed();
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
    onInputReturnPressed();
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
    onInputReturnPressed();
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
    onInputReturnPressed();
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
    onInputReturnPressed();
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
    onInputReturnPressed();
}

void MainWindow::onContextMenuZoomInTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto result = currentTab->result;
    auto text = result->toHtml();
    QString after;
    auto tokens = text.split(QString("font-size:") + QString::fromStdString(to_string(currentTab->fontSize)) + "px;");
    currentTab->fontSize += 1;
    for (auto i = 0; i < tokens.size(); ++i) {
        if (i < tokens.size() - 1)
            after += tokens.at(i) + QString("font-size:") + QString::fromStdString(to_string(currentTab->fontSize)) + "px;";
        else after += tokens.at(i);
    }
    currentTab->result->setHtml(after);
}

void MainWindow::onContextMenuZoomOutTriggered() {
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    auto result = currentTab->result;
    auto text = result->toHtml();
    QString after;
    auto tokens = text.split(QString("font-size:") + QString::fromStdString(to_string(currentTab->fontSize)) + "px;");
    currentTab->fontSize -= 1;
    for (auto i = 0; i < tokens.size(); ++i) {
        if (i < tokens.size() - 1)
            after += tokens.at(i) + QString("font-size:") + QString::fromStdString(to_string(currentTab->fontSize)) + "px;";
        else after += tokens.at(i);
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

void MainWindow::on_actionUser_Manual_triggered() {
    QDesktopServices::openUrl(QUrl("https://github.com/scottsanett/IceDict_QtWidgets/blob/master/documentation/Welcome%20to%20IceDict.md"));
}

void MainWindow::on_actionAbout_IceDict_triggered()
{
    auto aboutMessage =
            R"foo(
            <p align=center><h2>IceDict</h2></p>
            <p align=center style="font-weight: normal">Version )foo" + QString::fromUtf8(VERSION_STRING) + R"foo(</p>
            <p align=center style="font-weight: normal; font-size:11px">Copyright © 2017-2020 Li Xianpeng<br><br>Licensed under GNU LGPLv3 or later<br>All rights reserved.</p>)foo";
    auto messagebox = new QMessageBox(this);
    messagebox->setTextFormat(Qt::RichText);
    auto pixmap = QPixmap(":/alphabet/IceDict.png");
    pixmap = pixmap.scaledToHeight(128, Qt::SmoothTransformation);
    messagebox->setIconPixmap(pixmap);
    messagebox->setText(aboutMessage);
    messagebox->exec();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::on_actionAcknowledgements_triggered()
{
    auto acknowledgementMessage =
            R"foo(
            <p align=center><h1 id="toc_0">Acknowledgements</h1></p>
            <p style="font-weight:normal">IceDict utilizes the following copyrighted materials, the use of which is hereby acknowledged.</p>
            <h3 id="toc_1">Online Dictionary:</h3>
            <p style="font-weight:normal"><a href="http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?id=IcelOnline.IEOrd">Íslensk-ensk orðabók</a></p>
            <h3 id="toc_2">Offline Dictionaries:</h3>
            <p style="font-weight:normal"><a href="http://norse.ulver.com/dct/zoega/">A Concise Dictionary of Norse</a></p>
            <p style="font-weight:normal"><a href="http://norse.ulver.com/dct/cleasby/">An Icelandic-English Dictionary Based on the MS. Collections of the Late Richard Cleasby Enlarged and Completed by Gudbrand Vigfusson, M.A.</a></p>
            <h3 id="toc_3">Inflectional Data:</h3>
            <p style="font-weight:normal"><a href="http://bin.arnastofnun.is/forsida/">Beygingarlýsing í­slensks nútí­mamáls Stofnun Árna Magnússonar á­ í­slenskum fræðum</a></p>)foo";
    auto messageBox = new QMessageBox(this);
    messageBox->setWindowTitle("Acknowledgements");
    messageBox->setTextFormat(Qt::RichText);
    messageBox->setText(acknowledgementMessage);
    messageBox->exec();
}

void MainWindow::on_actionFind_in_Page_triggered()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());
    if (std::find(currentTab->flags.cbegin(), currentTab->flags.cend(), 1) == currentTab->flags.cend()) return;
    if (!currentTab->findPane) {
        currentTab->findPane = new FindPane(this);
        QObject::connect(currentTab->findPane, &FindPane::returnPressedSignal,
                         this, &MainWindow::searchPanelReturnPressed);
        QObject::connect(currentTab->findPane, &FindPane::prevButtonPressedSignal,
                         this, &MainWindow::searchPanePrevButtonPressed);
        QObject::connect(currentTab->findPane, &FindPane::nextButtonPressedSignal,
                         this, &MainWindow::searchPaneNextButtonPressed);
        currentTab->resultLayout->addWidget(currentTab->findPane->frame());
        currentTab->findPane->setFocus();
        currentTab->findPane->setFocusInput();
    }
    else if (currentTab->findPane->frame()->isHidden()) {
        currentTab->findPane->show();
        currentTab->findPane->setFocus();
        currentTab->findPane->setFocusInput();
    }
    else {
        currentTab->findPane->close();
    }
}

void MainWindow::on_actionShow_Status_Bar_triggered()
{
    if (statusBar->isHidden()) {
        statusBar->setHidden(false);
        ui->actionShow_Status_Bar->setText("Hide Status Bar");
    }
    else {
        statusBar->setHidden(true);
        ui->actionShow_Status_Bar->setText("Show Status Bar");
    }
}

void MainWindow::on_actionFind_triggered()
{
    activateInput();
}

void MainWindow::on_actionShow_Next_Tab_triggered()
{
    auto index = ui->resultsTab->currentIndex();
    if (index == ui->resultsTab->count() - 1) {
        index = 0;
    }
    else ++index;
    ui->resultsTab->setCurrentIndex(index);
}

void MainWindow::on_actionShow_Previous_Tab_triggered()
{
    auto index = ui->resultsTab->currentIndex();
    if (index == 0) {
        index = ui->resultsTab->count() - 1;
    }
    else --index;
    ui->resultsTab->setCurrentIndex(index);
}

void MainWindow::on_actionUndo_Close_Tab_triggered()
{
    if (!closedTabs.empty()) {
        auto tabToRestore = closedTabs.back();
        auto index = std::get<0>(tabToRestore);
        auto title = std::get<1>(tabToRestore);
        auto pimpl = std::get<2>(tabToRestore);
        ui->resultsTab->insertTab(index, pimpl->mainSplitter, title);
        ui->resultsTab->setCurrentIndex(index);
        tabIndices.insert(std::make_pair(pimpl->mainSplitter, pimpl));
        closedTabs.pop_back();
        if (closedTabs.empty()) ui->actionUndo_Close_Tab->setDisabled(true);
    }
}

void MainWindow::on_actionBack_triggered()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());

    if (--tabResultHistoryIndex.at(currentTab->mainSplitter) == 0) {
        ui->actionBack->setEnabled(false);
        currentTab->backButton->setEnabled(false);
        tabResultHistoryIndex.at(currentTab->mainSplitter) = 0;
    }

    currentTab->result->setHtml(tabResultHistory.at(currentTab->mainSplitter).at(tabResultHistoryIndex.at(currentTab->mainSplitter)));
    ui->actionForward->setEnabled(true);
    currentTab->nextButton->setEnabled(true);
}

void MainWindow::on_actionForward_triggered()
{
    auto currentTab = tabIndices.at(ui->resultsTab->currentWidget());

    if (++tabResultHistoryIndex.at(currentTab->mainSplitter) == tabResultHistory.at(currentTab->mainSplitter).size() - 1) {
        ui->actionForward->setEnabled(false);
        currentTab->nextButton->setEnabled(false);
        tabResultHistoryIndex.at(currentTab->mainSplitter) = tabResultHistory.at(currentTab->mainSplitter).size() - 1;
    }

    currentTab->result->setHtml(tabResultHistory.at(currentTab->mainSplitter).at(tabResultHistoryIndex.at(currentTab->mainSplitter)));
    ui->actionBack->setEnabled(true);
    currentTab->backButton->setEnabled(true);
}

void MainWindow::checkDatabaseIntegrity()
{
    updateDialog = new DBUpdateDialog(this);

    auto t = new QThread();
    updateDialogThread = new DBUpdateDialogThread();
    updateDialogThread->checkIntegrity(true);
    updateDialogThread->moveToThread(t);
    connect(updateDialogThread, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(t, SIGNAL(started()), updateDialogThread, SLOT(process()));
    connect(updateDialogThread, SIGNAL(finished()), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), updateDialogThread, SLOT(deleteLater()));
    connect(updateDialogThread, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(updateDialogThread, SIGNAL(finished()), this, SLOT(importBINDBs()));
    connect(updateDialogThread, SIGNAL(finished()), updateDialog, SLOT(close()));
    connect(updateDialogThread, SIGNAL(updateStatus(QString const)), updateDialog, SLOT(appendMsg(QString const)));
    connect(updateDialogThread, SIGNAL(signal_ShowProgress()), updateDialog, SLOT(slot_ShowProgress()));
    connect(updateDialogThread, SIGNAL(signal_UpdateProgress(qint64, qint64)), updateDialog, SLOT(slot_UpdateProgress(qint64, qint64)));
    connect(updateDialogThread, SIGNAL(signal_HideProgress()), updateDialog, SLOT(slot_HideProgress()));
    t->start();
}


void MainWindow::on_actionUpdate_Inflection_Database_triggered()
{
    updateDialog = new DBUpdateDialog(this);

    auto t = new QThread();
    updateDialogThread = new DBUpdateDialogThread();
    updateDialogThread->checkIntegrity(false);
    updateDialogThread->moveToThread(t);
    connect(updateDialogThread, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(t, SIGNAL(started()), updateDialogThread, SLOT(process()));
    connect(updateDialogThread, SIGNAL(finished()), t, SLOT(quit()));
    connect(t, SIGNAL(finished()), updateDialogThread, SLOT(deleteLater()));
    connect(updateDialogThread, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(updateDialogThread, SIGNAL(finished()), this, SLOT(importBINDBs()));
    connect(updateDialogThread, SIGNAL(finished()), updateDialog, SLOT(close()));
    connect(updateDialogThread, SIGNAL(updateStatus(QString const)), updateDialog, SLOT(appendMsg(QString const)));
    connect(updateDialogThread, SIGNAL(signal_ShowProgress()), updateDialog, SLOT(slot_ShowProgress()));
    connect(updateDialogThread, SIGNAL(signal_UpdateProgress(qint64, qint64)), updateDialog, SLOT(slot_UpdateProgress(qint64, qint64)));
    connect(updateDialogThread, SIGNAL(signal_HideProgress()), updateDialog, SLOT(slot_HideProgress()));
    t->start();
}

