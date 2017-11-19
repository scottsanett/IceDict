#ifndef TREEWIDGET_HPP
#define TREEWIDGET_HPP

#include <QObject>
#include <QTreeWidget>

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

signals:

public:
    using QTreeWidget::QTreeWidget;
    TreeWidget() = default;
};

#endif // TREEWIDGET_HPP
