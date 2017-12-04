#ifndef FINDPANE_HPP
#define FINDPANE_HPP

#include <QObject>
#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTextDocument>

class FindPane : public QWidget
{
    Q_OBJECT
public:
    explicit FindPane(QWidget *parent = nullptr);
private:
    QFrame * searchLayoutWidget;
    QHBoxLayout * searchLayout;
    QLabel * label;
    QLineEdit * searchPanel;
    QPushButton * prevButton;
    QPushButton * nextButton;
    QCheckBox * caseSensitivity;
    QCheckBox * matchWholeWords;
    QPushButton * closeButton;

    QTextDocument::FindFlags caseSensitivityFlag;
    QTextDocument::FindFlags matchWholeWordsFlag;

public:
    QFrame *  frame() { return searchLayoutWidget; }
    void setFocusInput();
    void show() { searchLayoutWidget->show(); }
    void close() { closeButtonPressed(); }
private slots:
    void returnPressed();
    void closeButtonPressed();
    void caseSensitivityChanged(int);
    void matchWholeWordsChanged(int);
signals:
    void returnPressedSignal(QString, QTextDocument::FindFlags);
    void nextButtonPressedSignal();
    void prevButtonPressedSignal();
public slots:
};

#endif // FINDPANE_HPP
