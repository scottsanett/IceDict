#include <QtDebug>
#include "TreeWidgetItem.hpp"

void TreeWidgetItem::adjustParentCheckStatus(QTreeWidgetItem * parent) {
    if (parent) {
        auto allUnchecked = true;
        auto allChecked = true;
        auto partiallyCheckExists = false;
        auto count = parent->childCount();
        if (count) {
            for (auto i = 0; i < count; ++i) {
                if (parent->child(i)->checkState(0) == Qt::Checked) {
                    allUnchecked = false;
                }
                else if (parent->child(i)->checkState(0) == Qt::Unchecked) {
                    allChecked = false;
                }
                else if (parent->child(i)->checkState(0) == Qt::PartiallyChecked) {
                    partiallyCheckExists = true;
                }
            }
        }
        if (allUnchecked && !allChecked && !partiallyCheckExists) {
            parent->setCheckState(0, Qt::Unchecked);
        }
        else if (allChecked && !partiallyCheckExists) {
            parent->setCheckState(0, Qt::Checked);
        }
        else if (partiallyCheckExists) {
            parent->setCheckState(0, Qt::PartiallyChecked);
        }
        else {
            parent->setCheckState(0, Qt::PartiallyChecked);
        }
    }
}

void TreeWidgetItem::setData(int column, int role, const QVariant& value) {
    const bool isCheckChange = column == 0
        && role == Qt::CheckStateRole
        && data(column, role).isValid()
        && checkState(0) != value;
    auto backup = checkState(0);
    QTreeWidgetItem::setData(column, role, value);

    if (isCheckChange) {
        auto count = this->childCount();
        auto parent = this->QTreeWidgetItem::parent();
        if (count) {
            if (checkState(0) == Qt::Checked) {
                for (auto i = 0; i < count; ++i) {
                    this->child(i)->setCheckState(0, Qt::Checked);
                }
                adjustParentCheckStatus(parent);
            }
            else if (checkState(0) == Qt::Unchecked) {
                for (auto i = 0; i < count; ++i) {
                    this->child(i)->setCheckState(0, Qt::Unchecked);
                }
                adjustParentCheckStatus(parent);
            }
            else if (checkState(0) == Qt::PartiallyChecked) {
                adjustParentCheckStatus(parent);
            }
        }
        else {
            adjustParentCheckStatus(parent);
        }

        // no child
        if (!count) {
            QVector<QString> result;
            QTreeWidgetItem * current = this;

            while (current) {
                result.push_front(current->text(0));
                current = current->QTreeWidgetItem::parent();
            }

            if (checkState(0) != Qt::PartiallyChecked && backup != Qt::PartiallyChecked) {
                emit checkStateChanged(checkState(0), result);
            }

        }
    }
}
