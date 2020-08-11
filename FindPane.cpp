#include "FindPane.hpp"

FindPane::FindPane(QWidget *parent) : QWidget(parent)
{
    searchLayoutWidget = new QFrame(parent);
    searchLayout = new QHBoxLayout(searchLayoutWidget);
    searchLayout->setContentsMargins(10, 0, 10, 0);
    searchLayoutWidget->setLayout(searchLayout);
    searchLayoutWidget->setFixedHeight(30);
    searchLayoutWidget->setFrameStyle(QFrame::NoFrame);

    label = new QLabel("Find:", parent);
    searchLayout->addWidget(label);
    searchPanel = new QLineEdit(parent);
    searchPanel->setClearButtonEnabled(true);
    searchPanel->setStyleSheet("font-size: 12px");
    searchLayout->addWidget(searchPanel);

#ifdef __APPLE__
    prevButton = new QPushButton("←", parent);
#else
    prevButton = new QPushButton("<", parent);
#endif
    prevButton->setFlat(true);
    prevButton->setFixedWidth(20);
    connect(prevButton, &QPushButton::pressed, this, &FindPane::prevButtonPressedSignal);
    searchLayout->addWidget(prevButton);
#ifdef __APPLE__
    nextButton = new QPushButton("→", parent);
#else
    nextButton = new QPushButton(">", parent);
#endif
    nextButton->setFlat(true);
    nextButton->setFixedWidth(20);
    connect(nextButton, &QPushButton::pressed, this, &FindPane::nextButtonPressedSignal);
    searchLayout->addWidget(nextButton);
    caseSensitivity = new QCheckBox("Case Sensitive", parent);
    connect(caseSensitivity, &QCheckBox::stateChanged, this, &FindPane::caseSensitivityChanged);
    searchLayout->addWidget(caseSensitivity);
    matchWholeWords = new QCheckBox("Match Whole Words", parent);
    connect(matchWholeWords, &QCheckBox::stateChanged, this, &FindPane::matchWholeWordsChanged);
    searchLayout->addWidget(matchWholeWords);
#ifdef __APPLE__
    closeButton = new QPushButton("✕", parent);
#else
    closeButton = new QPushButton("x", parent);
#endif
    closeButton->setFlat(true);
    closeButton->setFixedWidth(20);
    searchLayout->addWidget(closeButton);
    QObject::connect(closeButton, &QPushButton::pressed, this, &FindPane::closeButtonPressed);
    QObject::connect(searchPanel, &QLineEdit::returnPressed, this, &FindPane::returnPressed);
}

void FindPane::setFocusInput() {
    searchPanel->setFocus();
    searchPanel->selectAll();
}

void FindPane::closeButtonPressed() {
    searchLayoutWidget->hide();
    emit returnPressedSignal("", caseSensitivityFlag | matchWholeWordsFlag);
}

void FindPane::returnPressed() {
    emit returnPressedSignal(searchPanel->text(), caseSensitivityFlag | matchWholeWordsFlag);
}

void FindPane::caseSensitivityChanged(int value) {
    auto state = static_cast<Qt::CheckState>(value);
    if (state == Qt::Checked) {
        caseSensitivityFlag = QTextDocument::FindCaseSensitively;
    }
    else caseSensitivityFlag = 0x0;
    emit returnPressedSignal(searchPanel->text(), caseSensitivityFlag | matchWholeWordsFlag);
}

void FindPane::matchWholeWordsChanged(int value) {
    auto state = static_cast<Qt::CheckState>(value);
    if (state == Qt::Checked) {
        matchWholeWordsFlag = QTextDocument::FindWholeWords;
    }
    else matchWholeWordsFlag = 0x0;
    emit returnPressedSignal(searchPanel->text(), caseSensitivityFlag | matchWholeWordsFlag);
}
