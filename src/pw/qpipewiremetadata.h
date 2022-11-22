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

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

#include <QObject>

class QPipewire;

class QPipewireMetadata : public QObject
{
    Q_OBJECT
signals:
    void onAllKeysRemoved(uint32_t id);
    void onKeyRemoved(uint32_t id, const char* key);
    void onKeyUpdated(uint32_t id, const char* key, const char* type, const char* value);


protected:
    QPipewire *pipewire = nullptr;
    struct pw_metadata *metadata = nullptr;
    struct spa_hook metadata_listener;

public:
    explicit QPipewireMetadata(QPipewire *parent, uint32_t id, const spa_dict* props);
    virtual ~QPipewireMetadata();

    void setProperty(const char *key, const char *value);
    void clear();

    int _metadata_property(uint32_t id,
                           const char* key,
                           const char* type,
                           const char* value);
};
