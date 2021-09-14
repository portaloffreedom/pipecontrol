#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QList>

#include "src/qpipewirenode.h"

class QPipewireNodeListModel : public QAbstractListModel
{
    Q_OBJECT

    enum NodeRoles {
        IndexRole = Qt::UserRole + 1,
        NodeRole
    };

private:
    QList<QPipewireNode*> m_nodes;

public:
    explicit QPipewireNodeListModel(QObject *parent = nullptr);
    virtual ~QPipewireNodeListModel();

    inline QList<QPipewireNode*> &list() { return m_nodes; }
    inline const QList<QPipewireNode*> &constList() const { return m_nodes; }

    void sortList();

public:
    // Delegate methods from m_nodes
    void append(QPipewireNode *node);
    bool removeOne(QPipewireNode *node);
    inline int size() const { return m_nodes.size(); }
    inline QPipewireNode*& operator[](int index) { return m_nodes[index]; }
    inline QPipewireNode* const& operator[](int index) const { return m_nodes[index]; }

public:
    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex& index = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    //virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual bool insertRows(int row, int count, const QModelIndex& parent) override;
    virtual bool removeRows(int row, int count, const QModelIndex& parent) override;
    virtual QHash<int, QByteArray> roleNames() const override;
};
