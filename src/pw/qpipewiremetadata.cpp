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
#include "qpipewiremetadata.h"

#include <QDebug>
#include "src/pw/qpipewire.h"

#define QPIPEWIRE_CAST(x) QPipewireMetadata* _this = static_cast<QPipewireMetadata*>(x);

//-----------------------------------------------------------------------------
static int metadata_property(void *data,
                             uint32_t id,
                             const char* key,
                             const char* type,
                             const char* value)
{
    QPIPEWIRE_CAST(data);
    return _this->_metadata_property(id, key, type, value);
}

int QPipewireMetadata::_metadata_property(uint32_t id,
                                          const char* key,
                                          const char* type,
                                          const char* value)
{
    if (key == nullptr) {
        emit onAllKeysRemoved(id);
    } else if (value == nullptr) {
        emit onKeyRemoved(id, key);
    } else {
        emit onKeyUpdated(id, key, type, value);
    }

    return 0;
}

static const pw_metadata_events metadata_events {
    .version = PW_VERSION_METADATA_EVENTS,
    .property = metadata_property,
};

//-----------------------------------------------------------------------------

QPipewireMetadata::QPipewireMetadata(QPipewire *parent, uint32_t id, const spa_dict* props)
    : QObject(parent)
    , pipewire(parent)
{
    metadata = static_cast<pw_metadata*>(
                   pw_registry_bind(parent->registry, id, PW_TYPE_INTERFACE_Metadata, PW_VERSION_METADATA, 0));
    pw_metadata_add_listener(metadata,
                             &metadata_listener,
                             &metadata_events,
                             this);
    parent->resync();
}

QPipewireMetadata::~QPipewireMetadata()
{
    spa_hook_remove(&metadata_listener);
    if (metadata != nullptr) {
        pw_proxy_destroy((struct pw_proxy*) metadata);
    }
}

//-----------------------------------------------------------------------------

void QPipewireMetadata::setProperty(const char *key, const char *value)
{
    pw_metadata_set_property(metadata,
                             0,
                             key,
                             nullptr,
                             value);
    pipewire->round_trip();
}
