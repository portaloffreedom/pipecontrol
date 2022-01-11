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
#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QList>

#include "src/pw/qpipewirenode.h"

class QPipewireNodeListModel : public QAbstractListModel
{
    Q_OBJECT

    enum NodeRoles {
        IndexRole = Qt::UserRole + 1,
        NodeRole,
        IDRole,
        ActiveRole,
        RateRole,
        QuantumRole,
        WaitRole,
        BusyRole,
        NameRole,
        DriverIDRole,
    };
    const unsigned int N_ROLES = 10;

private:
    QList<QPipewireNode*> m_nodes;

public:
    explicit QPipewireNodeListModel(QObject *parent = nullptr);
    virtual ~QPipewireNodeListModel();

    inline QList<QPipewireNode*> &list() { return m_nodes; }
    inline const QList<QPipewireNode*> &constList() const { return m_nodes; }

    Q_INVOKABLE
    void sortList();

public:
    // Delegate methods from m_nodes
    void append(QPipewireNode *node);
    bool removeOne(QPipewireNode *node);
    bool removeAt(int index);
    inline int size() const { return m_nodes.size(); }
    inline QPipewireNode*& operator[](int index) { return m_nodes[index]; }
    inline QPipewireNode* const& operator[](int index) const { return m_nodes[index]; }

public:
    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex& index = QModelIndex()) const override;
    //virtual int columnCount(const QModelIndex& index = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual bool insertRows(int row, int count, const QModelIndex& parent) override;
    virtual bool removeRows(int row, int count, const QModelIndex& parent) override;
    virtual QHash<int, QByteArray> roleNames() const override;

    void rowChanged(QPipewireNode* node, int role);

    Q_INVOKABLE
    void move(int oldIndex, int newIndex);
};
