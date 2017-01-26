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
    importInflections();
    std::string zname = "zoega";
    std::string vname = "vifgusson";
    std::thread t_zoega(&MainWindow::importWordIndexThread, this, definitions, zname, 0);
    std::thread t_vifgusson(&MainWindow::importWordIndexThread, this, definitions, vname, 1);
    t_zoega.join(); t_vifgusson.join();
    ui->setupUi(this);
    this->setWindowTitle("Icelandic Dictionary");
    QIcon icon(":/alphabet/icon.ico");
    this->setWindowIcon(icon);
    ui->input->setPlaceholderText("Press a button first...");
    addTab = new QToolButton;
    connect(addTab, SIGNAL(clicked(bool)), this, SLOT(addTab_clicked()));
    addTab->setText("+");
    ui->resultsTab->setTabsClosable(true);
    connect(ui->resultsTab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    addTab->setStyleSheet("font-family: 'Segoe UI'; font-size: 11pt; background-color: rgba(255, 255, 255, 0)");
    ui->resultsTab->setCornerWidget(addTab, Qt::TopLeftCorner);
    addTab_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTab_clicked() {
    QTextBrowser * result = new QTextBrowser;
    result->setHtml(startScreen.c_str());
    ui->input->clear();
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

void MainWindow::buttonChangeColor() {
    ui->searchIcl->setStyleSheet("");
    ui->searchTxt->setStyleSheet("");
    ui->onlineIcl->setStyleSheet("");
    ui->onlineTxt->setStyleSheet("");
    ui->searchOrig->setStyleSheet("");
    ui->plot->setStyleSheet("");
    ui->printOne->setStyleSheet("");
    if (flags[0] == 1) { ui->searchIcl->setStyleSheet("background-color: grey"); }
    else if (flags[1] == 1) { ui->searchTxt->setStyleSheet("background-color: grey"); }
    else if (flags[2] == 1) { ui->onlineIcl->setStyleSheet("background-color: grey"); }
    else if (flags[3] == 1) { ui->searchOrig->setStyleSheet("background-color: grey"); }
    else if (flags[4] == 1) { ui->plot->setStyleSheet("background-color: grey"); }
    else if (flags[5] == 1) { ui->printOne->setStyleSheet("background-color: grey"); }
    else if (flags[6] == 1) { ui->onlineTxt->setStyleSheet("background-color: grey"); }
    else{}
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

void MainWindow::on_searchIcl_clicked()
{
    flags = {1, 0, 0, 0, 0, 0, 0};
    buttonChangeColor();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
}

void MainWindow::on_onlineIcl_clicked()
{
    flags = {0, 0, 1, 0, 0, 0, 0};
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    onlineEntries.clear();
    ui->input->clear();
    buttonChangeColor();
    ui->input->setPlaceholderText(tr("Insert word here..."));
}


void MainWindow::on_onlineTxt_clicked()
{
    flags = {0, 0, 0, 0, 0, 0, 1};
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    onlineEntries.clear();
    ui->input->clear();
    buttonChangeColor();
    ui->input->setPlaceholderText(tr("Insert text here..."));
}


void MainWindow::on_searchTxt_clicked()
{
    flags = {0, 1, 0, 0, 0, 0, 0};
    buttonChangeColor();
    ui->input->setPlaceholderText("Insert text here...");
    ui->input->clear();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
}

void MainWindow::on_searchOrig_clicked()
{
    flags = {0, 0, 0, 1, 0, 0, 0};
    buttonChangeColor();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
}

void MainWindow::on_plot_clicked()
{
    flags = {0, 0, 0, 0, 1, 0, 0};
    buttonChangeColor();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
    ui->options->clear();
    ui->input->setPlaceholderText(tr("Insert word here..."));
    ui->input->clear();
}


void MainWindow::on_printOne_clicked()
{
    typeTimes = 0;
    flags = {0, 0, 0, 0, 0, 1, 0};
    buttonChangeColor();
    stored.clear();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->clear();
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
//            qDebug() << ui->options->count();
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
//            auto results = ptrvecstrvecptr_t(new vecstrvecptr_t);
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

void MainWindow::allFormsAutocompleteThread(ptrvecstrvecptr_t results, std::string word, size_t index) {
    auto thisResult = results->at(index);
}

/*  Transfer a string to lower case   */
std::string MainWindow::toLower(std::string str) {
    const std::vector<std::string> upper = { "Þ", "Ð", "Æ", "Ö", "Á", "É", "Í", "Ó", "Ú", "Ý", "Œ" };
    const std::vector<std::string> lower = { "þ", "ð", "æ", "ö", "á", "é", "í", "ó", "ú", "ý", "œ" };
    auto f = [&](std::string & name) { for (auto i = 0; i < name.length(); ++i) {
        if (isalpha(name[i])) {
            if (isupper(name[i])) {
                tolower(name[i]);
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
    return str;
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
    ui->input->clear();
    std::string word = "<b>" + words + "</b>";
    auto zisspair = dictionaries->at(0)->equal_range(word);
    auto visspair = dictionaries->at(1)->equal_range(word);
    if ((zisspair.first == zisspair.second) && (visspair.first == visspair.second)) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setFontFamily("Perpetua");
        tabActive->setFontPointSize(20);
        tabActive->setText("Word not found.");
        return;
    }
    for(auto itr = zisspair.first; itr != zisspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = *itr->second;
        definitionResults.push_back(std::make_pair(key, thisEntry));
    }
    for (auto itr = visspair.first; itr != visspair.second; ++itr) {
        auto key = itr->first;
        auto thisEntry = *itr->second;
        definitionResults.push_back(std::make_pair(key, thisEntry));
    }
    ui->options->clear();
    QStringList alternatives;
    for (auto && i : definitionResults) {
        std::string key = i.first.c_str();
        key = key.substr(3, key.length() - 7);
        alternatives.push_back(key.c_str());
    }
    ui->options->addItems(alternatives);
    findDefinitionPrint(0);
}

void MainWindow::findDefinitionPrint(size_t index) {
    auto thisEntry = definitionResults.at(index);
    std::string key = thisEntry.first;
    std::string value;
    for (auto i : thisEntry.second) {
        value += i + '\n';
    }
    std::string display = key + ' ' + value;
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 16pt;\">" + display + "</span></p>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(display.c_str());
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
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
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
    toprint = "<span style=\"font-family: Perpetua; font-size: 16pt;\"><p align=\"center\"><table border=\"1\" cellpadding=\"20\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint.c_str());
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

void MainWindow::textualSearchPrint(size_t index) {
    auto thisEntry = textualResults.at(index);
    std::string key = thisEntry.first;
    std::string display = thisEntry.second;
    display = "<p align=\"justify\"><span style=\"font-family: Perpetua; font-size: 16pt;\">" + display + "</span></p>";
//    qDebug() << display.c_str();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(display.c_str());
}

void MainWindow::textualSearchThread(/*ptrvecstrvecptr_t results, */std::string word, size_t index) {
    auto thisDic = dictionaries->at(index);
    for (auto && i : *thisDic) {
        std::string key = i.first;
        for (auto && j : *i.second) {
            auto pos = j.find(word);
            if (pos != std::string::npos) {
                auto subsitute = "<b><span style=\"color:#ff0000;\">" + word + "</span></b>";
                j.replace(pos, word.length(), subsitute);
                std::string value = key + ' ' + j;
//                qDebug() << value.c_str();
                textualResults.push_back(std::make_pair(key, value));
                break;
            }
        }
    }
}

void MainWindow::textualSearch(std::string const & word) {
    ui->input->clear();
    std::thread t0(&MainWindow::textualSearchThread, this, word, 0);
    std::thread t1(&MainWindow::textualSearchThread, this, word, 1);
    t0.join(); t1.join();
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
    findDefinitionPrint(0);
}


void MainWindow::printAll(std::string const & str) {
    ui->input->clear();
    auto word = str + ';';
    auto & results = resultsToPrint;
    std::thread t0(&MainWindow::printAllThread, this, word, 0);
    std::thread t1(&MainWindow::printAllThread, this, word, 1);
    std::thread t2(&MainWindow::printAllThread, this, word, 2);
    std::thread t3(&MainWindow::printAllThread, this, word, 3);
    std::thread t4(&MainWindow::printAllThread, this, word, 4);
    std::thread t5(&MainWindow::printAllThread, this, word, 5);
    std::thread t6(&MainWindow::printAllThread, this, word, 6);
    t0.join();
    t1.join(); t2.join(); t3.join(); t4.join(); t5.join(); t6.join();
    auto resultSize = [&]() { size_t sz = 0; for (auto i : results) { sz += i.second.size(); } return sz; }();
    if (resultSize == 0) {
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setText("Word not found.");
        return;
    }
    for (auto && i : results) {
            ui->options->addItem(i.first.c_str());
    }
    printAllPrint(0);
}

void MainWindow::printAllThread(std::string word, size_t index) {
    auto thisDic = originals->at(index);
    auto & thisResult = resultsToPrint; // pointer to a vector of
    auto range = thisDic->equal_range(word);
    auto count = thisDic->count(word);
    if (count == 0) { return; }
    std::string filename = ":/alphabet/sources/part" + std::to_string(index + 1);
    QFile file(filename.c_str());
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line, partOfSpeech;
    auto currentPos = 0;
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

void MainWindow::printAllPrint(size_t index) {
    auto thisResult = resultsToPrint[index];
    std::string toprint;
    for (auto i : thisResult.second) {
       std::string temp = addStyleToResults(i);
       toprint += temp;
    }
    toprint = "<span style=\"font-family: Perpetua; font-size: 16pt;\"><p align=\"center\"><table border=\"1\" cellpadding=\"20\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint.c_str());
}

void MainWindow::printOneThread(ptrvecstrvecptr_t results, std::string word, std::string form, size_t index) {
    auto thisDic = originals->at(index);
    auto thisResult = results->at(index);
    auto range = thisDic->equal_range(word);
    auto count = thisDic->count(word);
    if (count == 0) { return; }
    std::string filename = ":/alphabet/sources/part" + std::to_string(index + 1);
    QFile file(filename.c_str());
    file.open(QIODevice::ReadOnly);
    auto qfile = file.readAll();
    std::istringstream issfile(qfile.toStdString());
    std::string line;
    auto currentPos = 0;
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
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->setText("Word not found.");
        ui->input->clear();
        return;
    }
    std::string toprint;
    for (auto && i : * results) {
        for (auto && j : *i) {
            j = addStyleToResults(j);
            toprint += j + '\n';
        }
    }
    toprint = "<span style=\"font-family: Perpetua; font-size: 16pt;\"><p align=\"center\"><table border=\"1\" cellpadding=\"20\">" + toprint + "</table></p></span>";
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    tabActive->setHtml(toprint.c_str());
    typeTimes = 0;
    ui->input->clear();
}


void MainWindow::on_input_editingFinished()
{
    std::string word = ui->input->text().toStdString();
    if (flags[0] == 1 && word.length() > 0) {
        std::string tag = "3. " + word;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
//        ui->options->clear();
        findDefinition(word);
    }
    else if (flags[1] == 1 && word.length() > 0) {
        std::string tag = "4. " + word;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        ui->options->clear();
        textualSearch(word);
    }
    else if (flags[2] == 1 && word.length() > 0) {
        std::string tag = "1. " + word;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        ui->options->clear();
        onlineEntries.clear();
        inputted = word;
        onlineDefinition(word);

    }
    else if (flags[3] == 1 && word.length() > 0) {
        std::string tag = "5. " + word;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        findInflection(word);
    }
    else if (flags[4] == 1 && word.length() > 0) {
        std::string tag = "6. " + word;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        ui->options->clear();
        resultsToPrint.clear();
        printAll(word);
    }
    else if (flags[5] == 1 && word.length() > 0) {
        ++typeTimes;
        if (typeTimes > 0) {
            ui->input->clear();
            ui->input->setPlaceholderText("Insert form here...");
            std::string tag = "7. " + word;
            ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        }
        else {
            ui->input->clear();
            ui->input->setPlaceholderText("Insert word here...");
        }
    }
    else if (flags[6] == 1 && word.length() > 0) {
        std::string tag = "2. " + word;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        ui->options->clear();
        onlineEntries.clear();
        inputted = word;
        onlineText(word);
    }
}

void MainWindow::on_options_itemClicked(QListWidgetItem *item)
{
    if (flags[0] == 1) {
        ui->input->setText(item->text());
//        qDebug() << item->text();
        std::string tag = "3. " + item->text().toStdString();
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        if (definitionResults.size() == 0) {
            findDefinition(item->text().toStdString());
        }
        else {
            size_t index = ui->options->currentRow();
            findDefinitionPrint(index);
        }
    }
    else if (flags[1] == 1) {
        ui->input->setText(item->text());
        std::string tag = "4. " + item->text().toStdString();
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        if (textualResults.size() == 0) {
            textualSearch(item->text().toStdString());
        }
        else {
            size_t index = ui->options->currentRow();
            textualSearchPrint(index);
        }
    }
    else if (flags[2] == 1) {
        std::string key = item->text().toStdString();
        std::string tag = "1. " + key;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        std::string url;
        auto itr = onlineEntries.find(key);
        if (itr != onlineEntries.end()) {
            url = itr->second;
        }
        downloadPage(url);
    }
    else if (flags[4] == 1) {
        std::string tag = "6. " + item->text().toStdString();
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
        tabActive->clear();
        size_t index = ui->options->currentRow();
        printAllPrint(index);

// a function that prints according to the index.
    }
    else if (flags[5] == 1) {
        std::string tag = "7. " + item->text().toStdString();
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        ui->input->setText(item->text());
        printOneForm = item->text().toStdString();
        printOne(printOneWord, printOneForm);
    }
    else if (flags[6] == 1) {
        std::string key = item->text().toStdString();
        std::string tag = "2. " + key;
        ui->resultsTab->setTabText(ui->resultsTab->currentIndex(), tag.c_str());
        std::string url;
        auto itr = onlineEntries.find(key);
        if (itr != onlineEntries.end()) {
            url = itr->second;
        }
        downloadPage(url);
    }
}

void MainWindow::downloadPage(std::string url) {
    ui->input->clear();
    std::string input = ui->input->text().toStdString();
    QUrl pageUrl(url.c_str());
    pageControl = new PageDownloader(pageUrl, this);
    connect(pageControl, SIGNAL(downloaded()), this, SLOT(loadPage()));
}

void MainWindow::loadPage() {
    QByteArray qPage = (pageControl->downloadedData());
    QString str = QString::fromLatin1(qPage);
    webpage = str.toStdString();
    parsePage();
    auto * tabActive = dynamic_cast<QTextBrowser*>(ui->resultsTab->currentWidget());
    webpage = "<span style=\"font-family: Perpetua; font-size: 16pt;\">" + webpage + "</span>";
    tabActive->setHtml(webpage.c_str());
    webpage.clear();
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

void MainWindow::parsePage() {
    if (webpage.find("produced no results.") != std::string::npos) {
        auto pos = webpage.find("<h3>While searching in Icelandic Online Dictionary and Readings</h3>");
        if (pos != std::string::npos) webpage = webpage.substr(pos, webpage.length() - pos);
        pos = webpage.find("<div class=\"mainBackground\">");
        if (pos != std::string::npos) webpage = webpage.substr(0, pos);
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
    }
}
