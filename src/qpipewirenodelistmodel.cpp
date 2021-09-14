#include "qpipewirenodelistmodel.h"
#include <QDebug>

QPipewireNodeListModel::QPipewireNodeListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QPipewireNodeListModel::~QPipewireNodeListModel()
{}

void QPipewireNodeListModel::sortList()
{
//    beginMoveRows(QModelIndex(), 0, m_nodes.size()-1, QModelIndex(), 0);
//    emit layoutAboutToBeChanged();
//    const QList<QPipewireNode*> old = m_nodes;
//    m_nodes.clear();
//    for (QPipewireNode* device : old) {
//        if (device->driver() == nullptr) {
//            m_nodes.append(device);
//            for(QPipewireNode* client : old) {
//                if (device == client->driver()) {
//                    m_nodes.append(client);
//                }
//            }
//        }
//    }

//    emit layoutChanged();
//    emit changePersistentIndex(QModelIndex(), QModelIndex());
//    endMoveRows();

    // Find elements to move
    std::list<std::pair<QPipewireNode*, int>> moving;
    for (int i=0; i<m_nodes.size(); i++)
    {
        QPipewireNode* node = m_nodes[i];
        QPipewireNode* driver = node->driver();

        if (driver == nullptr)
        {
            // I'm a driver, no need to be moved
            continue;
        }

        for(int j=i-1; j >= 0; j--)
        {
            // Search for driver in reverse
            QPipewireNode* parent = m_nodes[j];
            if (parent->driver() == driver) continue; // all good, keep going up
            if (parent == driver) {
                // found, all good, exit
                break;
            } else {
                // not found
                moving.emplace_back(node, i);
                break;
            }
        }
    }

    const QList<QPipewireNode*> old = m_nodes;

    // Resort unparented elements
    while(!moving.empty())
    {
        auto [node,index] = moving.front();
        QPipewireNode* driver = node->driver();
        assert(driver != nullptr);
        int i=0;
        for (; i<old.size(); i++)
        { // Find driver
            QPipewireNode* node = m_nodes[i];
            if (node == driver) {
                break;
            }
        }
        assert(i < old.size()); // Driver not found???

        // Find new position
        int target = i;
//        while(target+1 < m_nodes.size() && m_nodes[target+1]->driver() == driver) {
//            target++;
//        }

        // Move element
        beginMoveRows(createIndex(index,0,node), index, index, createIndex(target, 0, node), target);
        emit layoutAboutToBeChanged();
        m_nodes.move(index, target);
        emit layoutChanged();
//        emit changePersistentIndex(,,);
        endMoveRows();

        moving.pop_front();
        // refix all indexes in moving
        for (auto &iter : moving) {
            if (index < target) {
                if (iter.second < index) continue;
                if (iter.second > target) continue;
                target--; // everyone shifts front
            } else {
                if (iter.second > index) continue;
                if (iter.second < target) continue;
                target++; // everyone shits back
            }
        }
    }
}

void QPipewireNodeListModel::append(QPipewireNode* node)
{
    beginInsertRows(QModelIndex(), m_nodes.size(), m_nodes.size());
    node->connect(node, &QPipewireNode::driverChanged, this, &QPipewireNodeListModel::sortList);
    m_nodes.append(node);
    endInsertRows();
    sortList();
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
        return vNode;
    }
    default:
        qWarning() << "UNDEFINED ROLE" << _role;
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
        { IndexRole, "index" },
        { NodeRole, "node" },
    };
}
