#include "StatusBar.hpp"

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{

}

void StatusBar::showMessage(QString msg) {
    auto spaceCount = (this->width() - msg.length() * 4.5) / (2 * 4.5);
    for (auto i = 0; i < spaceCount; ++i) {
        msg = " " + msg;
    }
    this->QStatusBar::clearMessage();
    this->QStatusBar::showMessage(msg);
}

void StatusBar::resizeEvent(QResizeEvent *) {
    auto msg = this->currentMessage();
    msg = msg.trimmed();
    auto spaceCount = (this->width() - msg.length() * 4.5) / (2 * 4.5);
    for (auto i = 0; i < spaceCount; ++i) {
        msg = " " + msg;
    }
    this->QStatusBar::clearMessage();
    this->QStatusBar::showMessage(msg);
}
