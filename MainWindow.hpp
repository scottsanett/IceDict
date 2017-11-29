#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QObject>
#include <QFile>
#include "QWidget"
#include "QMainWindow"
#include "QPushButton"
#include "QLineEdit"
#include "QLabel"
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
#include "QSplitter"
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
#include <type_traits>
#include <cctype>

#include "PageDownloader.hpp"
#include "TreeWidget.hpp"
#include "TreeWidgetItem.hpp"
#include "inflection.hpp"
#include "macros.hpp"

using map_t = std::multimap<QString, int>;
using mapptr_t = std::shared_ptr<map_t>;
using mapptrvec_t = QVector<mapptr_t>;
using mapptrvecptr_t = std::shared_ptr<mapptrvec_t>;
using strset_t = std::set<QString>;
using strsetptr_t = std::shared_ptr<strset_t>;
using strptr_t = std::shared_ptr<QString>;
using strvec_t = QVector<QString>;
using strvecptr_t = std::shared_ptr<strvec_t>;
using vecstrvecptr_t = QVector<strvecptr_t>;
using ptrvecstrvecptr_t = std::shared_ptr<vecstrvecptr_t>;
using strvecptrmap_t = std::multimap<QString, strvecptr_t>;
using strvecptrmapptr_t = std::shared_ptr<strvecptrmap_t>;
using strvecptrmapptrvec_t = QVector<strvecptrmapptr_t>;
using strvecptrmapptrvecptr_t = std::shared_ptr<strvecptrmapptrvec_t>;
using vecpair_t = QVector<std::pair<QString, strvec_t>>;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    struct Pimpl {
#ifdef __APPLE__
        size_t perpetuaFontSize = 40;
        size_t segoeFontSize = 14;
#else
        size_t perpetuaFontSize = 25;
        size_t segoeFontSize = 28;
#endif

        std::array<bool, 6> flags = {{0, 0, 0, 0, 0, 0}};
        QString webpage;
        QString inputted;
        std::map<QString, QString> onlineEntries;
        std::set<QVector<QString>> inflStruct;
        QVector<QString> inflSelectResult;
        QVector<std::pair<QString, QString>> textualResults;
        QVector<std::pair<QString, QVector<QString>>> definitionResults;
        QVector<std::pair<QString, QVector<QString>>> resultsToPrint;

        QVBoxLayout * centralLayout;
        QSplitter * mainSplitter;
        QSplitter * inputLayout;
        QLineEdit * input;
        QListWidget * options;
        QTextBrowser * result;
        PageDownloader * pageControl;
        QListWidget * resultsFromDictionaries;
        TreeWidget * inflectionForms;
    };

    /* pointers */
    Ui::MainWindow * ui;
    PageDownloader * pageControl;
//    QMenu * resultContextMenu;
    Inflection InflManager;

    /* POD members */
    int tabIndex;
    const char * startScreen = "<html><head/><body><p align=\"center\"><br/></p><p align=\"center\"><span style=\" font-family: 'Perpetua'; font-size:24px;\">Welcome to IceDict</span></p><p align=\"center\"><span style=\" font-size:20px;\">ᚢᛁᛚᚴᚢᛉᛁᚾ᛬ᛏᛁᛚ᛬ᚢᚱᚦᛅᛒᚢᚴᛅᛣ᛬ᛁᛋᛚᛁᚾᛋᚴᚱᛅᛣ </span></p><p align=\"center\"><br/></p><p align=\"center\"><img src=\":/alphabet/cover.jpg\"/></p></body></html>";
    const char * writeUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=lemma&q1=";
    const char * writeUrl2 = "&submit=Search";
    const char * textUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=dentry"
                            "&q1=";
    const char * textUrl2 = "&submit=Search";

    enum class POS { Noun, Verb, Adjective, Adverb, Pronoun, Numerical};
    enum POS currentPOS;

    struct PartOfSpeech {
        const char * MasNoun = "<td>kk</td>";
        const char * FemNoun = "<td>kvk</td>";
        const char * NetNoun = "<td>hk</td>";
        const char * Adjective = "<td>lo</td>";
        const char * Verb = "<td>so</td>";
        const char * Pronoun = "<td>fn</td>";
        const char * Adverb = "<td>ao</td>";
        const char * Numerical = "<td>to</td>";
    } PartOfSpeech;

    /* Abstract data types */
    std::map<QString, QString> writeRules = {
        std::make_pair("á", "%E1"), std::make_pair("é", "%E9"), std::make_pair("í", "%ED"), std::make_pair("ó", "%F3"),
        std::make_pair("ú", "%FA"), std::make_pair("ý", "%FD"), std::make_pair("Á", "%C1"), std::make_pair("É", "%C9"),
        std::make_pair("Í", "%CD"), std::make_pair("Ó", "%D3"), std::make_pair("Ú", "%DA"), std::make_pair("Ý", "%DD"),
        std::make_pair("æ", "%E6"), std::make_pair("ö", "%F6"), std::make_pair("Æ", "%C6"), std::make_pair("Ö", "%D6"),
        std::make_pair("þ", "%FE"), std::make_pair("ð", "%F0"), std::make_pair("Þ", "%DE"), std::make_pair("Ð", "%D0"),
    };

//    QList<std::shared_ptr<Pimpl>> tabs;
    std::map<QWidget*, std::shared_ptr<Pimpl>> tabIndices;

    mapptrvecptr_t inflectionals;
    mapptrvecptr_t definitions;
    mapptrvecptr_t originals;
    strvecptrmapptrvecptr_t dictionaries;
    strsetptr_t forms;
    strsetptr_t wordindex;

private slots:
    void search_norse_word();
    void search_norse_text();
    void search_original();
    void search_all_inflections();
    void search_icelandic_word();
    void search_icelandic_text();

    void loadPage();
    void addTab_clicked();
    void closeTab(int);

    /* slots concerning selection changes in the InflectionForms tree */
    void checkStateChanged(Qt::CheckState, QVector<QString> const);
    void onInputTextEdited(const QString &arg1);
    void onInputEditingFinished();
    void onOptionsItemClicked(QListWidgetItem *item);
    void resultsFromDictionariesItemClicked(QListWidgetItem * item);

    void onResultContextMenuRequested(QPoint const & p);

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

    void onContextMenuIceToEngTriggered();
    void onContextMenuEngToIceTriggered();
    void onContextMenuNorToEngTriggered();
    void onContextMenuEngToNorTriggered();
    void onContextMenuSearchInfReverseTriggered();
    void onContextMenuSearchInfTriggered();

    void onContextMenuZoomInTriggered();
    void onContextMenuZoomOutTriggered();


    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

private:
    TreeWidgetItem * constructItem(QString, TreeWidget * parent);
    TreeWidgetItem * constructItem(QString, TreeWidgetItem * parent);

    QString addStyleToResults(QString str);
    QString wordToWrite(QString);

    /* functions concerning user interface */
    void activateInput();
    void initializeResultFromDictionaries();
    void clearResultFromDictionaries();
    void initializeInflectionForms();
    void clearInflectionForms();
    void newTab();

    void fillInflectionForms(QString const &);

    /* functions concerning inflectional menu */
    /* first level */
    void fillVerbs(QString const &);
    void fillNouns(QString const &);
    void fillAdjectives(QString const &);
    void fillPronouns(QString const &);
    void fillAdverbs(QString const &);

    /* second level, templates */
    template <bool D,
              Infl::Category cat,
              Infl::Forms ... args,
              class T = typename std::conditional<D, TreeWidget, TreeWidgetItem>::type>
    void fillStructures(T * item, QString const &);

    template <int Size, Infl::Forms ... Forms>
    std::array<bool, Size> fillStructuresHelper(QString const &);

    template <int Size, Infl::Forms ... forms>
    void fillStructureItemConstructor(TreeWidgetItem * item, std::array<bool, Size> const &);

    /* CheckInfo Parser Functions */
    QVector<QString> ParseCheckStateChangeInfo(); // returns new result to print
    QVector<QString> ParseNoun();
    QVector<QString> ParseVerb();
    QVector<QString> ParseAdjective();
    QVector<QString> ParseAdverb();
    QVector<QString> ParsePronoun();

    bool findImpersonal(QString const &);
    bool findActive(QString const &);
    bool findMiddle(QString const &);
    bool findIndicative(QString const &);
    bool findSubjunctive(QString const &);
    bool findImperative(QString const &);
    bool findPresent(QString const &);
    bool findPast(QString const &);
    bool findFstPrs(QString const &);
    bool findSndPrs(QString const &);

    QVector<QString> regexResult(QString const &, QString const &);

    /* functions concerning online query */
    void downloadPage(QString url);
    bool parsePage();

    /* importing functions */
    void importWordIndex();
    void importInflections();
    void importInflectionsThread(mapptrvecptr_t mapvec, size_t i);
    void importWordIndexThread(mapptrvecptr_t mapvec, QString const name, size_t i);
    void importOriginal();
    void importOriginalThread(mapptrvecptr_t mapvec, size_t i);
    void importDictionary();
    void importDictionaryThread(QString const name, size_t i);

    /* query functions */
    void findDefinition(QString const & word);
    void findDefinitionPrint(size_t index);
    void findInflection(QString const & word);
    void onlineDefinition(QString const & word);
    void onlineText(QString const & word);
    void findInflectionThread(ptrvecstrvecptr_t dics, QString word, size_t index);
    void textualSearch(QString const & word);
    void textualSearchThread(QString word, size_t index);
    void textualSearchPrint(size_t index);
    void printAll(QString const & word);
    void printAllThread(QString word, size_t index);
    void printAllPrint(size_t index);
    void allFormsAutocompleteThread(ptrvecstrvecptr_t dics, QString word, size_t index);
};


template <typename T>
std::string to_string(T value) {
    std::ostringstream os;
    os << value;
    return os.str() ;
}


#endif // MAINWINDOW_H
