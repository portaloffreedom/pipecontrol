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
    std::cout << "client: id("<<info->id<<")"<<std::endl;

    const struct spa_dict_item *item;
    spa_dict_for_each(item, info->props) {
        m_properties[item->key] = item->value;
    }

    std::cout << "\tprops" << std::endl;
    QMap<QString, QString>::const_iterator i = m_properties.constBegin();
    while (i != m_properties.constEnd()) {
        std::cout << "\t\t" << i.key().toStdString() << ":\t\"" << i.value().toStdString() << '"' << std::endl;
        ++i;
    }
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
