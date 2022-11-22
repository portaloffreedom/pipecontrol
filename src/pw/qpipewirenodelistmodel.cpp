/*
 * This file is part of the pipecontrol project.
 * Copyright (c) 2022 Matteo De Carlo.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "qpipewirenodelistmodel.h"
#include <QDebug>

QPipewireNodeListModel::QPipewireNodeListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

QPipewireNodeListModel::~QPipewireNodeListModel()
{}

void QPipewireNodeListModel::sortList()
{
   emit layoutAboutToBeChanged();
   QList<QPipewireNode*> old = m_nodes;
//   std::sort(old.begin(), old.end(), [](QPipewireNode*a, QPipewireNode*b) {
//       return a->nodeName() > b->nodeName();
//   });
   m_nodes.clear();
   for (QPipewireNode* device : old) {
       if (device->driver() == nullptr) {
           m_nodes.append(device);
           for(QPipewireNode* client : old) {
               if (device == client->driver()) {
                   m_nodes.append(client);
               }
           }
       }
   }

   emit layoutChanged();
}

#include <iostream>
void QPipewireNodeListModel::append(QPipewireNode* node)
{
    // qWarning() << "Adding new node " << node->name();
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
    m_nodes.append(node);
    endInsertRows();
    sortList();
}

bool QPipewireNodeListModel::removeOne(QPipewireNode* node)
{
    // qWarning() << "Removing node" << node->name();
    int index = m_nodes.indexOf(node);
    if (index != -1) {
        return removeAt(index);
    } else {
        return false;
    }
}

bool QPipewireNodeListModel::removeAt(int index)
{
    if (index >= 0 && index < m_nodes.size()) {
        beginRemoveRows(QModelIndex(), index, index);
        m_nodes.removeAt(index);
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

QVariant QPipewireNodeListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case IndexRole:
        return "Index";
    case NodeRole:
    case Qt::DisplayRole:
        return "Node";
    case IDRole:
        return QVariant("ID");
    case ActiveRole:
        return QVariant("Active");
    case RateRole:
        return QVariant("Rate");
    case QuantumRole:
        return QVariant("Quantum");
    case WaitRole:
        return QVariant("Wait");
    case BusyRole:
        return QVariant("Busy");
    case NameRole:
        return QVariant("Name");
    case DriverIDRole:
        return QVariant("Driver");
    default:
        qWarning() << "UNDEFINED ROLE" << role;
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

void QPipewireNodeListModel::move(int from, int to)
{
    if (from == to) return;
    int n=1;
    if (from > to) {
        // Only move forwards - flip if backwards moving
        int tfrom = from;
        int tto = to;
        from = tto;
        to = tto+n;
        n = tfrom-tto;
    }

    beginMoveRows(QModelIndex(), from, from+n-1, QModelIndex(), to+n);
    for (int i=n-1; i>=0; i--)
        m_nodes.move(from+i, to);
    endMoveRows();
}

