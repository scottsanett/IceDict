#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QObject>
#include <QFile>
#include "QPushButton"
#include "QLineEdit"
#include "QLabel"
#include "QDialog"
#include "QHBoxLayout"
#include "QTextBrowser"
#include "QResource"
#include "QTextStream"
#include "QDebug"
#include "QListWidgetItem"
#include <QCompleter>
#include "QStringList"
#include "QString"
#include "QIcon"
#include "QToolButton"
#include <QCheckBox>
#include <QSlider>
#include <QSpacerItem>
#include <QComboBox>
#include <QGroupBox>
#include <QTextBlock>
#include <QTextCursor>
#include "QShortcut"
#include "QMessageBox"
#include "QSplitter"
#include <QDesktopServices>

#include <cstdlib>
#include <thread>
#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <deque>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <cctype>

#include "FindPane.hpp"
#include "StatusBar.hpp"
#include "PageDownloader.hpp"
#include "TreeWidget.hpp"
#include "TreeWidgetItem.hpp"
#include "inflection.hpp"
#include "macros.hpp"
#include "dbupdater.hpp"

using map_t = std::multimap<QString, int>;
using vecmap_t = QVector<map_t>;  // QVector<std::multimap<QString, int>>
using setstr_t = std::set<QString>;
using vecstr_t = QVector<QString>;
using vecvecstr_t = QVector<vecstr_t>; // QVector<QVector<QString>>
using mapvecstr_t = std::multimap<QString, vecstr_t>; // std::multimap<QString, QVector<QString>>
using vecmapvecstr_t = QVector<mapvecstr_t>; // QVector<std::multimap<QString, QVector<QString>>>
using vecpair_t = QVector<std::pair<QString, vecstr_t>>; // QVector<std::pair<QString, QVector<QString>>>

struct QStringCaseInsensitiveComparator {
    bool operator() (QString const & lhs, QString const & rhs) const {
        if (QString::compare(lhs, rhs, Qt::CaseInsensitive) < 0) return true;
        else return false;
    }
};

namespace Ui {
class MainWindow;
}

namespace Infl {
    enum Transforms {
        Vowels = 1 << 0,
        Consonants = 1 << 1,
        Inflections = 1 << 2
    };

    inline Transforms operator& (Transforms a, Transforms b) {
        return static_cast<Transforms>(static_cast<int>(a) & static_cast<int>(b));
    }

    inline Transforms operator| (Transforms a, Transforms b) {
        return static_cast<Transforms>(static_cast<int>(a) | static_cast<int>(b));
    }

    enum class Results {No, One, Many, Maybe};
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    struct Pimpl {
        QString textInQuery;
        QString wordAfterRedirection;
        size_t fontSize = 15;
        std::array<bool, 6> flags = {{0, 0, 0, 0, 0, 0}};
        QString webpage;
        std::set<QVector<QString>> inflStruct;
        std::multimap<QString, QString> onlineEntries;
        std::multimap<QString, QString, QStringCaseInsensitiveComparator> textualResults;
        std::multimap<QString, QVector<QString>, QStringCaseInsensitiveComparator> definitionResults;
        QVector<QString> inflSelectResult;
        QVector<std::pair<QString, QVector<QString>>> resultsToPrint;

        QVBoxLayout * centralLayout;
        QSplitter * mainSplitter;
        QHBoxLayout * buttonLayout;
        QWidget * buttonLayoutWidget;
        QPushButton * backButton;
        QPushButton * nextButton;
        QSplitter * inputLayout;
        QSplitter * resultLayout;
        QVBoxLayout * inputPaneLayout;
        QWidget * inputPaneLayoutWidget;
        QComboBox * comboBox;
        QLineEdit * input;
        QListWidget * options;
        QTextBrowser * result;
        FindPane * findPane;

        TreeWidget * inflectionForms;
        QPushButton * proceedButton;

        ~Pimpl() {
            centralLayout->deleteLater();
            mainSplitter->deleteLater();
            buttonLayout->deleteLater();
            buttonLayoutWidget->deleteLater();
            backButton->deleteLater();
            nextButton->deleteLater();
            inputLayout->deleteLater();
            resultLayout->deleteLater();
            inputPaneLayout->deleteLater();
            inputPaneLayoutWidget->deleteLater();
            comboBox->deleteLater();
            input->deleteLater();
            options->deleteLater();
            result->deleteLater();
        }
    };


    /* pointers */
    Ui::MainWindow * ui;
    PageDownloader * pageControl;
    StatusBar * statusBar;
    Inflection InflManager;
    QCompleter * norseWordCompleter;
    DBDownloader * m_DBDownloader;
    DBDownloaderHelper * m_DBDownloadHelper;
//    QCompleter * inflectionalsCompleter;

    /* POD members */
    const char * searchUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=lemma&q1=";
    const char * searchUrl2 = "&submit=Search";
    const char * textUrl1 = "http://digicoll.library.wisc.edu/cgi-bin/IcelOnline/IcelOnline.TEId-idx?type=simple&size=First+100&rgn=dentry&q1=";
    const char * textUrl2 = "&submit=Search";
    const char * BINDBUrl = "https://bin.arnastofnun.is/django/api/nidurhal/?file=SHsnid.csv.zip";

#ifdef _WIN64
    const char * startScreen = "<html><head/><body><p align=\"center\"><br/></p><p align=\"center\"><span style=\"font-size:24px;\">Welcome to IceDict</span></p><p align=\"center\"<br/></p><p align=\"center\"><img src=\":/alphabet/cover.jpg\"/></p></body></html>";
#else
    const char * startScreen = "<html><head/><body><p align=\"center\"><br/></p><p align=\"center\"><span style=\"font-size:24px;\">Welcome to IceDict</span></p><p align=\"center\"><span style=\"font-size:20px;\">ᚢᛁᛚᚴᚢᛉᛁᚾ᛬ᛏᛁᛚ᛬ᚢᚱᚦᛅᛒᚢᚴᛅᛣ᛬ᛁᛋᛚᛁᚾᛋᚴᚱᛅᛣ</span></p><p align=\"center\"></p><p align=\"center\"><img src=\":/alphabet/cover.jpg\"/></p></body></html>";
#endif

    enum class POS { Noun, Verb, Adjective, Adverb, Pronoun, Numerical};
    enum POS currentPOS;


    struct PartOfSpeech {
        const char * MasNoun = " masc; ";
        const char * FemNoun = " fem; ";
        const char * NetNoun = " neut; ";
        const char * Adjective = " adj; ";
        const char * Verb = " verb; ";
        const char * Pronoun = " pron; ";
        const char * Adverb = " adv; ";
        const char * Numerical = " num; ";
    } PartOfSpeech;

    /* Abstract data types */
    std::array<QString, 15> vowels = {
        {"a", "á", "e", "é", "i", "í", "o", "ó", "u", "ú", "y", "ý", "ö", "æ"}
    };


    std::array<std::pair<QString, QString>, 20> writeRules = {
        {std::make_pair("á", "%E1"), std::make_pair("é", "%E9"), std::make_pair("í", "%ED"), std::make_pair("ó", "%F3"),
        std::make_pair("ú", "%FA"), std::make_pair("ý", "%FD"), std::make_pair("Á", "%C1"), std::make_pair("É", "%C9"),
        std::make_pair("Í", "%CD"), std::make_pair("Ó", "%D3"), std::make_pair("Ú", "%DA"), std::make_pair("Ý", "%DD"),
        std::make_pair("æ", "%E6"), std::make_pair("ö", "%F6"), std::make_pair("Æ", "%C6"), std::make_pair("Ö", "%D6"),
        std::make_pair("þ", "%FE"), std::make_pair("ð", "%F0"), std::make_pair("Þ", "%DE"), std::make_pair("Ð", "%D0")}
    };

    std::map<QWidget*, std::shared_ptr<Pimpl>> tabIndices;
    std::map<QWidget*, std::deque<QString>> tabResultHistory;
    std::map<QWidget*, int> tabResultHistoryIndex;
    std::deque<std::tuple<int, QString, std::shared_ptr<Pimpl>>> closedTabs;

    std::array<map_t, 8> inflectionals;
    std::array<map_t, 8> originals;
    std::array<map_t, 2> definitions;
    std::array<mapvecstr_t, 2> dictionaries;

    setstr_t wordindex;

    QString appDataLocation;
    QStringList wordIndexList;
    QList<QTextEdit::ExtraSelection> findInPageSelections;
    QList<QTextEdit::ExtraSelection>::size_type findInPageSelectionIndex;

    QNetworkAccessManager webControl;

private slots:
    void importBINDBs();

    void activateInput();
    void loadPage();
    void connectionError();
    void timeoutError();
    void addTab_clicked();
    void closeTab(int);
    void searchPanelReturnPressed(QString, QTextDocument::FindFlags);
    void searchPaneNextButtonPressed();
    void searchPanePrevButtonPressed();

    /* slots concerning selection changes in the InflectionForms tree */
    void checkStateChanged(Qt::CheckState, QVector<QString> const);
    void proceedButtonPressed();
    void onInputTextEdited(const QString &arg1);
    void onInputReturnPressed();
    void onOptionsItemClicked(QListWidgetItem *item);
    void onResultContextMenuRequested(QPoint const & p);
    void onResultUrlClicked(QUrl);
    void onResultTextChanged(QString);
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
    void onComboBoxIndexChanged(int index);
    void onTabCloseButtonClicked(int index);
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionUser_Manual_triggered();
    void on_actionAbout_IceDict_triggered();
    void on_actionAbout_Qt_triggered();
    void on_actionAcknowledgements_triggered();
    void on_actionFind_in_Page_triggered();
    void on_actionShow_Status_Bar_triggered();
    void on_actionFind_triggered();
    void on_actionShow_Next_Tab_triggered();
    void on_actionShow_Previous_Tab_triggered();
    void on_actionUndo_Close_Tab_triggered();
    void on_actionBack_triggered();
    void on_actionForward_triggered();

    void on_actionUpdate_Inflection_Database_triggered();

private:
    TreeWidgetItem * constructItem(QString, TreeWidget * parent);
    TreeWidgetItem * constructItem(QString, TreeWidgetItem * parent);

    QString addStyleToResults(QString str);
    QString wordToWrite(QString);
    QString oldToModern(QString, Infl::Transforms);
    QString removeNonAlpha(QString);
    bool isVowel(QChar ch);

    /* functions concerning user interface */
    void initializeInflectionForms();
    void clearInflectionForms();

    void fillInflectionForms(vecstr_t const &);

    /* functions concerning inflectional menu */
    /* first level */
    void fillVerbs(vecstr_t const &);
    void fillNouns(vecstr_t const &);
    void fillAdjectives(vecstr_t const &);
    void fillPronouns(vecstr_t const &);
    void fillAdverbs(vecstr_t const &);

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

    /* functions concerning online query */
    void downloadPage(QString url);
    Infl::Results parsePage();

    /* importing functions */
    void importWordIndex();
    void importInflections();
    void importInflectionsThread(std::array<map_t, 8> & mapvec, size_t i);
    void importOriginal();
    void importOriginalThread(std::array<map_t, 8> & mapvec, size_t i);
    void importDictionary();
    void importDictionaryThread(QString const name, size_t i);

    /* query functions */
    void findDefinition(QString word);
    void findDefinitionPrint(size_t index);
    void onlineDefinition(QString word);
    void onlineText(QString word);
    void findInflection(QString word);
    void findInflectionThread(std::array<vecstr_t, 8> & dics, QString word, size_t index);
    void textualSearch(QString const & word);
    void textualSearchThread(QString word, size_t index);
    void textualSearchPrint(size_t index);
    void printAll(QString word);
    void printAllThread(QString word, size_t index);
    void printAllPrint(size_t index);

    int examineBINDBs();
    QString hashFile(QString const &, QCryptographicHash::Algorithm = QCryptographicHash::Md5);
};


template <typename T>
std::string to_string(T value) {
    std::ostringstream os;
    os << value;
    return os.str() ;
}


#endif // MAINWINDOW_H
