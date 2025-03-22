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
#include "qpipewireclient.h"

#include "src/pw/qpipewire.h"

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
        const QString key = QString::fromUtf8(item->key);
        const QString value = QString::fromUtf8(item->value);
        m_properties[key] = value;
		// std::clog << "client_" << this->m_id << ":[" << key.toStdString() << "]=" << value.toStdString() << std::endl;
        Q_EMIT propertyChanged(key, value);
    }

    Q_EMIT propertiesChanged();
}

static const pw_client_events client_events {
    .version = PW_VERSION_CLIENT_EVENTS,
    .info = client_info,
};

//-----------------------------------------------------------------------------

QPipewireClient::QPipewireClient(QPipewire *parent, uint32_t id, const spa_dict* props)
    : QObject(parent)
    , pipewire(parent)
	, m_id(id)
{
    client = static_cast<pw_client*>(
                 pw_registry_bind(pipewire->registry, id, PW_TYPE_INTERFACE_Client, PW_VERSION_CLIENT, 0));

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
