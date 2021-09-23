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
        if (node == nullptr) {
            continue;
        }
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
            if (parent == nullptr || parent->driver() == driver) continue; // all good, keep going up
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

//    const QList<QPipewireNode*> old = m_nodes;

    // Resort unparented elements
    while(!moving.empty())
    {
        auto [node,source_i] = moving.front();
        QPipewireNode* driver = node->driver();
        assert(driver != nullptr);
        int i=0;
        for (; i<m_nodes.size(); i++)
        { // Find driver
            QPipewireNode* potential_parent = m_nodes[i];
            if (potential_parent == driver) {
                break;
            }
        }

        // Find new position
        int target = i;
        if (i >= m_nodes.size()) {
            // Driver not found???
            target = m_nodes.size()-1;
        }

        while(target+1 < m_nodes.size() && m_nodes[target+1]->driver() == driver) {
            target++;
        }

        // Move element
//        beginMoveRows(index(source_i).parent(), source_i, source_i, index(target).parent(), target);
        emit layoutAboutToBeChanged();
        m_nodes.move(source_i, target);
        emit layoutChanged();
//        emit changePersistentIndex(,,);
//        endMoveRows();

        moving.pop_front();
        // refix all indexes in moving
        for (auto &iter : moving) {
            if (source_i < target) {
                if (iter.second < source_i) continue;
                if (iter.second > target) continue;
                target--; // everyone shifts front
            } else {
                if (iter.second > source_i) continue;
                if (iter.second < target) continue;
                target++; // everyone shits back
            }
        }
    }
}

#include <iostream>
void QPipewireNodeListModel::append(QPipewireNode* node)
{
    qWarning() << "Adding new node " << node->name();
    beginInsertRows(QModelIndex(), m_nodes.size(), m_nodes.size());
    node->connect(node, &QPipewireNode::idChanged, this, [this, node]() {this->rowChanged(node, IDRole); });
    node->connect(node, &QPipewireNode::activeChanged, this, [this, node]() {this->rowChanged(node, ActiveRole);});
    node->connect(node, &QPipewireNode::rateChanged, this, [this, node]() {this->rowChanged(node, RateRole); });
    node->connect(node, &QPipewireNode::quantumChanged, this, [this, node]() {this->rowChanged(node, QuantumRole); });
    node->connect(node, &QPipewireNode::waitingChanged, this, [this, node]() {this->rowChanged(node, WaitRole); });
    node->connect(node, &QPipewireNode::busyChanged, this, [this, node]() {this->rowChanged(node, BusyRole); });
    node->connect(node, &QPipewireNode::nameChanged, this, [this, node]() {this->rowChanged(node, NameRole); });
    node->connect(node, &QPipewireNode::driverChanged, this, [this, node]() {this->rowChanged(node, DriverIDRole);});
    node->connect(node, &QPipewireNode::driverChanged, this, &QPipewireNodeListModel::sortList);
    emit layoutAboutToBeChanged();
    m_nodes.append(node);
    emit layoutChanged();
    endInsertRows();
    sortList();
}

bool QPipewireNodeListModel::removeOne(QPipewireNode* node)
{
    qWarning() << "Removing node" << node->name();
    int index = m_nodes.indexOf(node);
    if (index != -1) {
        return removeOne(index);
    } else {
        return false;
    }
}

bool QPipewireNodeListModel::removeOne(int index)
{
    if (index >= 0 && index < m_nodes.size()) {
        beginRemoveRows(this->index(index), index, index);
        emit layoutAboutToBeChanged();
        m_nodes.removeAt(index);
        emit layoutChanged();
        endRemoveRows();
        return true;
    } else {
        return false;
    }
}

int QPipewireNodeListModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_nodes.size();
}

QVariant QPipewireNodeListModel::data(const QModelIndex &index, int _role) const
{
    int role = NodeRoles(_role);
    int i = index.row();
    QPipewireNode *node = m_nodes.at(i);
    switch (role) {
    case IndexRole:
        return i;
    case NodeRole:
    case Qt::DisplayRole:
    {
        if (node) {
            QVariant vNode;
            vNode.setValue(node);
            return vNode;
        } else {
            return QVariant(QVariant::Invalid);
        }
    }
    case IDRole:
        return node ? node->id() : QVariant("nullptr");
    case ActiveRole:
        return node ? node->active() : false;
    case RateRole:
        return node ? node->rate() : QVariant("nullptr");
    case QuantumRole:
        return node ? node->quantum() : QVariant("nullptr");
    case WaitRole:
        return node ? node->waiting() : 0.0;
    case BusyRole:
        return node ? node->busy() : 0.0;
    case NameRole:
        return node ? node->name() : "[deleted]";
    case DriverIDRole:
        if (node && node->driver()) {
            return node->driver()->id();
        } else {
            return -1;
        }
    default:
        qWarning() << "UNDEFINED ROLE" << _role;
        throw std::runtime_error("Undefined role");
    }
}

bool QPipewireNodeListModel::insertRows(int position, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, position, position+count-1);

    for(int i=0; i<count; ++i) {
        m_nodes.insert(position, nullptr);
    }

    endInsertRows();
    return true;
}

bool QPipewireNodeListModel::removeRows(int position, int count, const QModelIndex& /*parent*/)
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
        { Qt::DisplayRole, "display"},
        { IDRole, "id" },
        { ActiveRole, "active" },
        { RateRole, "rate" },
        { QuantumRole, "quantum" },
        { WaitRole, "wait" },
        { BusyRole, "busy" },
        { NameRole, "name" },
        { DriverIDRole, "driverID" },
    };
}

void QPipewireNodeListModel::rowChanged(QPipewireNode* node, int role)
{
    int index = m_nodes.indexOf(node);
    QModelIndex topLeft = createIndex(index, 0);
    QModelIndex bottomRight = createIndex(index, 0);
    emit dataChanged(topLeft, bottomRight, {role});
}
