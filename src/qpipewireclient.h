#ifndef QPIPEWIRECLIENT_H
#define QPIPEWIRECLIENT_H

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

#include <QObject>
#include <QMap>
#include <QString>

class QPipewire;

class QPipewireClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMap<QString,QString>* properties READ properties)
signals:

private:
    QPipewire *pipewire = nullptr;

    struct pw_client *client = nullptr;
    struct spa_hook client_listener;

    QMap<QString,QString> m_properties;

public:
    explicit QPipewireClient(QPipewire *parent, uint32_t id, const char *type);
    virtual ~QPipewireClient();

    QMap<QString,QString> *properties() { return &m_properties; }

    void _client_info(const struct pw_client_info *info);
};

#endif // QPIPEWIRECLIENT_H
