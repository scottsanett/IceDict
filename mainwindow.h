#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QObject>
#include <QFile>
#include "QWidget"
#include "QMainWindow"
#include "QMenuBar"
#include "QStatusBar"
#include "QPushButton"
#include "QLineEdit"
#include "QHBoxLayout"
#include "QGridLayout"
#include "QTextBrowser"
#include "QResource"
#include "QTextStream"
#include "QDebug"
#include "QListWidgetItem"
#include "QStringList"
#include "QString"
#include "QIcon"

#include <thread>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <cctype>

#include "dialog.h"
#include "pagedownloader.h"

using map_t = std::multimap<std::string, size_t>;
using mapptr_t = std::shared_ptr<map_t>;
using mapptrvec_t = std::vector<mapptr_t>;
using mapptrvecptr_t = std::shared_ptr<mapptrvec_t>;
using strset_t = std::set<std::string>;
using strsetptr_t = std::shared_ptr<strset_t>;
using strptr_t = std::shared_ptr<std::string>;
using strvec_t = std::vector<std::string>;
using strvecptr_t = std::shared_ptr<strvec_t>;
using vecstrvecptr_t = std::vector<strvecptr_t>;
using ptrvecstrvecptr_t = std::shared_ptr<vecstrvecptr_t>;
using strvecptrmap_t = std::multimap<std::string, strvecptr_t>;
using strvecptrmapptr_t = std::shared_ptr<strvecptrmap_t>;
using strvecptrmapptrvec_t = std::vector<strvecptrmapptr_t>;
using strvecptrmapptrvecptr_t = std::shared_ptr<strvecptrmapptrvec_t>;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_searchIcl_clicked();
    void on_input_textEdited(const QString &arg1);
    void on_input_editingFinished();
    void on_options_itemClicked(QListWidgetItem *item);
    void on_searchOrig_clicked();
    void on_searchTxt_clicked();
    void on_plot_clicked();
    void on_printOne_clicked();
    void on_onlineIcl_clicked();
    void on_onlineTxt_clicked();
    void loadPage();
//    void on_input_textChanged(const QString &arg1);
//    void on_input_returnPressed();

private:
    PageDownloader * pageControl;
    std::string webpage;
    std::string inputted;
    std::map<std::string, std::string> writeRules = {
        std::make_pair("á", "%E1"), std::make_pair("é", "%E9"), std::make_pair("í", "%ED"), std::make_pair("ó", "%F3"),
        std::make_pair("ú", "%FA"), std::make_pair("ý", "%FD"), std::make_pair("Á", "%C1"), std::make_pair("É", "%C9"),
        std::make_pair("Í", "%CD"), std::make_pair("Ó", "%D3"), std::make_pair("Ú", "%DA"), std::make_pair("Ý", "%DD"),
        std::make_pair("æ", "%E6"), std::make_pair("ö", "%F6"), std::make_pair("Æ", "%C6"), std::make_pair("Ö", "%D6"),
        std::make_pair("þ", "%FE"), std::make_pair("ð", "%F0"), std::make_pair("Þ", "%DE"), std::make_pair("Ð", "%D0"),
    };
    std::vector<std::pair<std::string, std::string>> readRules = {
        std::make_pair("AE1", "æ"), std::make_pair("TH1", "þ"), std::make_pair("A1", "á"), std::make_pair("E1", "é"), std::make_pair("I1", "í"), std::make_pair("O1", "ó"),
        std::make_pair("U1", "ú"), std::make_pair("Y1", "ý"), std::make_pair("O3", "ö"), std::make_pair("D1", "ð")
    };
    std::string writeUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=lemma&q1=";
    std::string writeUrl2 = "&submit=Search";
    std::string textUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=dentry&q1=";
    std::string textUrl2 = "&submit=Search";
    std::string printOneWord;
    std::string printOneForm;
    QStringList dictentries;
    bool inflectionReady = false;
    bool textReady = false;
    size_t typeTimes = 0;
    Ui::MainWindow * ui;
    std::vector<std::string> stored;
    std::vector<bool> flags = {0, 0, 0, 0, 0, 0, 0};
    mapptrvecptr_t inflectionals;
    mapptrvecptr_t definitions;
    mapptrvecptr_t originals;
    strvecptrmapptrvecptr_t dictionaries;
    strsetptr_t forms;
    strsetptr_t wordindex;
    strsetptr_t allInflectionalWords;
    std::string addStyleToResults(std::string str);
    std::string wordToWrite(std::string);
    std::string wordToRead(std::string);
    std::map<std::string, std::string> onlineEntries;
    void downloadPage(std::string url);
    void parsePage();
    void buttonChangeColor();
    void importWordIndex();
    void importInflections();
    void importForms();
    void importInflectionsThread(mapptrvecptr_t mapvec, size_t i);
    void importWordIndexThread(mapptrvecptr_t mapvec, std::string const name, size_t i);
    void importOriginal();
    void importOriginalThread(mapptrvecptr_t mapvec, size_t i);
    void importDictionary();
    void importDictionaryThread(std::string const name, size_t i);
    std::string toLower(std::string str);
    void findDefinition(std::string const & word);
    void findInflection(std::string const & word);
    void onlineDefinition(std::string const & word);
    void onlineText(std::string const & word);
    void findInflectionThread(ptrvecstrvecptr_t dics, std::string word, size_t index);
    void textualSearch(std::string const & word);
    void textualSearchThread(ptrvecstrvecptr_t dics, std::string word, size_t index);
    void printAll(std::string const & word);
    void printAllThread(ptrvecstrvecptr_t dics, std::string word, size_t index);
    void printOne(std::string const & word, std::string const & form);
    void printOneThread(ptrvecstrvecptr_t dics, std::string word, std::string form, size_t index);
    void allFormsAutocompleteThread(ptrvecstrvecptr_t dics, std::string word, size_t index);
};


#endif // MAINWINDOW_H
