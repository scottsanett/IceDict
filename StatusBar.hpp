#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStatusBar>

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);
    void showMessage(QString);
protected:
    virtual void resizeEvent(QResizeEvent * event) override;

signals:

public slots:
};

#endif // STATUSBAR_HPP
