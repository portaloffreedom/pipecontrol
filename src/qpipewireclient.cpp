#include "qpipewireclient.h"

#include "src/qpipewire.h"

#include <iostream>
#include <QString>

#define QPIPEWIRE_CAST(x) QPipewireClient* _this = static_cast<QPipewireClient*>(x);

//-----------------------------------------------------------------------------

static void client_info(void *data, const pw_client_info *info)
{
    QPIPEWIRE_CAST(data);
    _this->_client_info(info);
}

void QPipewireClient::_client_info(const struct pw_client_info *info)
{
    const struct spa_dict_item *item;
    spa_dict_for_each(item, info->props) {
        const QString key = item->key;
        const QString value = item->value;
        m_properties[key] = value;
        emit propertyChanged(key, value);
    }

    emit propertiesChanged();
}

static const pw_client_events client_events {
    .version = PW_VERSION_CLIENT_EVENTS,
    .info = client_info,
};

//-----------------------------------------------------------------------------

QPipewireClient::QPipewireClient(QPipewire *parent, uint32_t id, const char *type)
    : QObject(parent)
    , pipewire(parent)
{
    client = static_cast<pw_client*>(
                 pw_registry_bind(pipewire->registry, id, type, PW_VERSION_CLIENT, 0));

    if (client == nullptr) {
        throw std::runtime_error("Error creating client proxy");
    }

    pw_client_add_listener(client,
                           &client_listener,
                           &client_events,
                           this);
    pipewire->resync();
}

QPipewireClient::~QPipewireClient()
{
    spa_hook_remove(&client_listener);
    if (client != nullptr) {
        pw_proxy_destroy((struct pw_proxy*) client);
    }
}
