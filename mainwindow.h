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

#include <regex>
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

#include "dialog.h"
#include "entry.hpp"
#include "inflection.hpp"

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
//    void on_searchReg_clicked();
    void on_plot_clicked();
    void on_printOne_clicked();

private:
    std::string printOneWord;
    std::string printOneForm;
    QStringList dictentries;
    bool inflectionReady = false;
    bool textReady = false;
    size_t typeTimes = 0;
    Ui::MainWindow * ui;
    std::vector<std::string> stored;
    std::vector<bool> flags = {0, 0, 0, 0, 0, 0};
    mapptrvecptr_t inflectionals;
    mapptrvecptr_t definitions;
    mapptrvecptr_t originals;
    strvecptrmapptrvecptr_t dictionaries;
    strsetptr_t forms;
    strsetptr_t wordindex;
    strsetptr_t allInflectionalWords;
    void addStyleToResults(std::string & str);
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
    void toLower(std::string & str);
    void findDefinition(std::string const & word);
    void findInflection(std::string const & word);
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
