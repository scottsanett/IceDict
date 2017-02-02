#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QObject>
#include <QFile>
#include "QWidget"
#include "QMainWindow"
//#include "QMenuBar"
//#include "QStatusBar"
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
#include "QToolButton"
#include <cstdlib>

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
using vecpair_t = std::vector<std::pair<std::string, strvec_t>>;

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
    void addTab_clicked();
    void closeTab(int);

private:
    QToolButton * addTab;
    PageDownloader * pageControl;
    std::string webpage;
    std::string inputted;
    std::string startScreen = "<html><head/><body><p align=\"center\"><br/></p><p align=\"center\"><span style=\" font-family: 'Perpetua'; font-size:24pt;\">Welcome to the Icelandic Dictionary</span></p><p align=\"center\"><span style=\" font-size:20pt;\">ᚢᛁᛚᚴᚢᛉᛁᚾ᛬ᛏᛁᛚ᛬ᚢᚱᚦᛅᛒᚢᚴᛅᛣ᛬ᛁᛋᛚᛁᚾᛋᚴᚱᛅᛣ</span></p><p align=\"center\"><br/></p><p align=\"center\"><img src=\":/alphabet/cover.jpg\"/></p></body></html>";
    std::map<std::string, std::string> writeRules = {
        std::make_pair("á", "%E1"), std::make_pair("é", "%E9"), std::make_pair("í", "%ED"), std::make_pair("ó", "%F3"),
        std::make_pair("ú", "%FA"), std::make_pair("ý", "%FD"), std::make_pair("Á", "%C1"), std::make_pair("É", "%C9"),
        std::make_pair("Í", "%CD"), std::make_pair("Ó", "%D3"), std::make_pair("Ú", "%DA"), std::make_pair("Ý", "%DD"),
        std::make_pair("æ", "%E6"), std::make_pair("ö", "%F6"), std::make_pair("Æ", "%C6"), std::make_pair("Ö", "%D6"),
        std::make_pair("þ", "%FE"), std::make_pair("ð", "%F0"), std::make_pair("Þ", "%DE"), std::make_pair("Ð", "%D0"),
    };
    std::string writeUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=lemma&q1=";
    std::string writeUrl2 = "&submit=Search";
    std::string textUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=dentry&q1=";
    std::string textUrl2 = "&submit=Search";
    std::string printOneWord;
    std::string printOneForm;
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
    vecpair_t resultsToPrint;
    std::string addStyleToResults(std::string str);
    std::string wordToWrite(std::string);
    std::map<std::string, std::string> onlineEntries;
    std::vector<std::pair<std::string, std::vector<std::string>>> definitionResults;
    std::vector<std::pair<std::string, std::string>> textualResults;
    void newTab();
    void downloadPage(std::string url);
    void parsePage();
    void buttonChangeColor();
    void importWordIndex();
    void importForms();
    void importInflections();
    void importInflectionsThread(mapptrvecptr_t mapvec, size_t i);
    void importWordIndexThread(mapptrvecptr_t mapvec, std::string const name, size_t i);
    void importOriginal();
    void importOriginalThread(mapptrvecptr_t mapvec, size_t i);
    void importDictionary();
    void importDictionaryThread(std::string const name, size_t i);
    void findDefinition(std::string const & word);
    void findDefinitionPrint(size_t index);
    void findInflection(std::string const & word);
    void onlineDefinition(std::string const & word);
    void onlineText(std::string const & word);
    void findInflectionThread(ptrvecstrvecptr_t dics, std::string word, size_t index);
    void textualSearch(std::string const & word);
    void textualSearchThread(std::string word, size_t index);
    void textualSearchPrint(size_t index);
    void printAll(std::string const & word);
    void printAllThread(std::string word, size_t index);
    void printAllPrint(size_t index);
    void printOne(std::string const & word, std::string const & form);
    void printOneThread(ptrvecstrvecptr_t dics, std::string word, std::string form, size_t index);
    void allFormsAutocompleteThread(ptrvecstrvecptr_t dics, std::string word, size_t index);
};


template <typename T>
std::string to_string(T value)
{
  //create an output string stream
  std::ostringstream os ;

  //throw the value into the string stream
  os << value ;

  //convert the string stream into a string and return
  return os.str() ;
}


#endif // MAINWINDOW_H
