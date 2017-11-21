#ifndef STANDARDITEM_HPP
#define STANDARDITEM_HPP

#include <QObject>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>

class TreeWidgetItem: public QObject, public QTreeWidgetItem
{
    Q_OBJECT

private:
    void adjustParentCheckStatus(QTreeWidgetItem * parent);

signals:
    void checkStateChanged(Qt::CheckState, QVector<QString> const);

public:
    using QTreeWidgetItem::QTreeWidgetItem;
    TreeWidgetItem() = default;
    virtual void setData(int column, int role, const QVariant& value) override;

};

#endif // STANDARDITEM_HPP
