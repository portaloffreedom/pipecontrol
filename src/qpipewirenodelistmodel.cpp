#include "qpipewirenodelistmodel.h"

QPipewireNodeListModel::QPipewireNodeListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QPipewireNodeListModel::~QPipewireNodeListModel()
{}

void QPipewireNodeListModel::append(QPipewireNode* node)
{
    beginInsertRows(QModelIndex(), m_nodes.size(), m_nodes.size());
    m_nodes.append(node);
    endInsertRows();
}

bool QPipewireNodeListModel::removeOne(QPipewireNode* node)
{
    int index = m_nodes.indexOf(node);
    if (index != -1) {
        beginRemoveRows(QModelIndex(), index, index);
        m_nodes.removeAt(index);
        endRemoveRows();
        return true;
    } else {
        return false;
    }
}

int QPipewireNodeListModel::rowCount(const QModelIndex &parent) const
{
    return m_nodes.size();
}

//int QPipewireNodeListModel::columnCount(const QModelIndex& index) const
//{
//    return 10;
//}

#include <iostream>
QVariant QPipewireNodeListModel::data(const QModelIndex &index, int _role) const
{
    int role = NodeRoles(_role);
    int i = index.row();
    switch (role) {
    case IndexRole:
        return i;
    case NodeRole:
    case Qt::DisplayRole:
    {
        QPipewireNode *node = m_nodes.at(i);
        QVariant vNode;
        vNode.setValue(node);
//        switch (index.column()) {
//        case 0:
//            vNode.setValue(QString(node->active() ? "!" : ""));
//            break;
//        case 1:
//            vNode.setValue(node->id());
//            break;
//        case 2:
//            vNode.setValue(node->quantum());
//            break;
//        case 3:
//            vNode.setValue(node->rate());
//            break;
//        case 4:
//            vNode.setValue(node->waiting());
//            break;
//        case 5:
//            vNode.setValue(node->busy());
//            break;
//        case 6:
//            vNode.setValue(node->id());
//            break;
//        case 7:
//            vNode.setValue(node->error());
//            break;
//        case 8:
//            vNode.setValue(node->xrun());
//            break;
//        case 9:
//            vNode.setValue(node->name());
//            break;
//        default:
//            std::cerr << "WHAT COL? " << index.column() << std::endl;
//            vNode.setValue(QString("diocane"));
//        }
        return vNode;
    }
    default:
        std::cerr << "UNDEFINED ROLE" << _role << std::endl;
        throw std::runtime_error("Undefined role");
    }
}

bool QPipewireNodeListModel::insertRows(int position, int count, const QModelIndex& parent)
{
    beginInsertRows(QModelIndex(), position, position+count-1);

    for(int i=0; i<count; ++i) {
        m_nodes.insert(position, nullptr);
    }

    endInsertRows();
    return true;
}

bool QPipewireNodeListModel::removeRows(int position, int count, const QModelIndex& parent)
{
    beginRemoveRows(QModelIndex(), position, position+count-1);

    for(int i=0; i<count; ++i) {
        m_nodes.removeAt(position);
    }

    endRemoveRows();
    return true;
}

QHash<int, QByteArray> QPipewireNodeListModel::roleNames() const
{
    return {
        { IndexRole, "index"},
        { NodeRole, "node"},
    };
}
