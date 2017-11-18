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
#include "QShortcut"
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

using map_t = std::multimap<std::string, int>;
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
    void on_input_textEdited(const QString &arg1);
    void on_input_editingFinished();
    void on_options_itemClicked(QListWidgetItem *item);

    void search_norse_word();
    void search_norse_text();
    void search_original();
    void search_all_inflections();
    void search_one_inflection();
    void search_icelandic_word();
    void search_icelandic_text();
    void resultsFromDictionaries_itemClicked(QListWidgetItem * item);

    void loadPage();
    void addTab_clicked();
    void closeTab(int);

    void on_actionMinimize_triggered();

    void on_actionFullscreen_triggered();

    void on_actionNew_Tab_triggered();

    void on_actionClose_Tab_triggered();

    void on_actionModern_Icelandic_triggered();

    void on_actionEnglish_Modern_Icelandic_triggered();

    void on_actionOld_Icelandic_English_triggered();

    void on_actionOld_Icelandic_Text_Search_triggered();

    void on_actionSearch_Inflections_triggered();

    void on_actionList_All_Forms_triggered();

    void on_actionList_One_Specific_Form_triggered();

private:
    PageDownloader * pageControl;
    QListWidget * resultsFromDictionaries;
    Ui::MainWindow * ui;

    bool textReady = false;
    int typeTimes = 0;
    const char * startScreen = "<html><head/><body><p align=\"center\"><br/></p><p align=\"center\"><span style=\" font-family: 'Perpetua'; font-size:24pt;\">Welcome to IceDict</span></p><p align=\"center\"><span style=\" font-size:20pt;\">ᚢᛁᛚᚴᚢᛉᛁᚾ᛬ᛏᛁᛚ᛬ᚢᚱᚦᛅᛒᚢᚴᛅᛣ᛬ᛁᛋᛚᛁᚾᛋᚴᚱᛅᛣ</span></p><p align=\"center\"><br/></p><p align=\"center\"><img src=\":/alphabet/cover.jpg\"/></p></body></html>";
    const char * writeUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=lemma&q1=";
    const char * writeUrl2 = "&submit=Search";
    const char * textUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=dentry&q1=";
    const char * textUrl2 = "&submit=Search";

    std::map<std::string, std::string> writeRules = {
        std::make_pair("á", "%E1"), std::make_pair("é", "%E9"), std::make_pair("í", "%ED"), std::make_pair("ó", "%F3"),
        std::make_pair("ú", "%FA"), std::make_pair("ý", "%FD"), std::make_pair("Á", "%C1"), std::make_pair("É", "%C9"),
        std::make_pair("Í", "%CD"), std::make_pair("Ó", "%D3"), std::make_pair("Ú", "%DA"), std::make_pair("Ý", "%DD"),
        std::make_pair("æ", "%E6"), std::make_pair("ö", "%F6"), std::make_pair("Æ", "%C6"), std::make_pair("Ö", "%D6"),
        std::make_pair("þ", "%FE"), std::make_pair("ð", "%F0"), std::make_pair("Þ", "%DE"), std::make_pair("Ð", "%D0"),
    };

    std::map<std::string, std::string> letterMapping = {
      std::make_pair("á", "Á"), std::make_pair("é", "É"), std::make_pair("í", "Í"),
        std::make_pair("ó", "Ó"), std::make_pair("ú", "Ú"), std::make_pair("ý", "Ý"),
        std::make_pair("", ""), std::make_pair("", ""), std::make_pair("", "")
    };

    std::string webpage;
    std::string inputted;
    std::string printOneWord;
    std::string printOneForm;
    std::vector<std::string> stored;
    std::vector<bool> flags = {0, 0, 0, 0, 0, 0, 0};
    mapptrvecptr_t inflectionals;
    mapptrvecptr_t definitions;
    mapptrvecptr_t originals;
    strvecptrmapptrvecptr_t dictionaries;
    strsetptr_t forms;
    strsetptr_t wordindex;
    vecpair_t resultsToPrint;
    std::map<std::string, std::string> onlineEntries;
    std::vector<std::pair<std::string, std::vector<std::string>>> definitionResults;
    std::vector<std::pair<std::string, std::string>> textualResults;

private:
    std::string addStyleToResults(std::string str);
    std::string wordToWrite(std::string);

    void activateInput();
    void clearResultFromDictionaries();
    void newTab();
    void downloadPage(std::string url);
    bool parsePage();
    void buttonChangeColor();
    void importWordIndex();
    void importForms();
    void importInflections();
    void importInflectionsThread(mapptrvecptr_t mapvec, int i);
    void importWordIndexThread(mapptrvecptr_t mapvec, std::string const name, int i);
    void importOriginal();
    void importOriginalThread(mapptrvecptr_t mapvec, int i);
    void importDictionary();
    void importDictionaryThread(std::string const name, int i);
    void findDefinition(std::string const & word);
    void findDefinitionPrint(int index);
    void findInflection(std::string const & word);
    void onlineDefinition(std::string const & word);
    void onlineText(std::string const & word);
    void findInflectionThread(ptrvecstrvecptr_t dics, std::string word, int index);
    void textualSearch(std::string const & word);
    void textualSearchThread(std::string word, int index);
    void textualSearchPrint(int index);
    void printAll(std::string const & word);
    void printAllThread(std::string word, int index);
    void printAllPrint(int index);
    void printOne(std::string const & word, std::string const & form);
    void printOneThread(ptrvecstrvecptr_t dics, std::string word, std::string form, int index);
    void allFormsAutocompleteThread(ptrvecstrvecptr_t dics, std::string word, int index);
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
