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

#ifndef QPIPEWIREDEVICE_H
#define QPIPEWIREDEVICE_H

#include <qobject.h>
#include <spa/utils/hook.h>

struct pw_device;
class QPipewire;

/**
 * @todo write docs
 */
class QPipewireDevice : public QObject
{
Q_OBJECT
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)

Q_SIGNALS:
    void volumeChanged(float);

public:
    /**
     * Default constructor
     */
    QPipewireDevice(QPipewire *parent, uint32_t id, const struct spa_dict *props);

    /**
     * Destructor
     */
    virtual ~QPipewireDevice();

    void _device_info (const struct pw_device_info *info);

    void _device_param (int seq, uint32_t id, uint32_t index, uint32_t next,
                        const struct spa_pod *param);

    // setters getters
    float volume() const { return m_volume; }
    void setVolume(float v) { m_volume = v; /*TODO*/ }

private:
    const QPipewire* pipewire;
    const uint32_t m_id;
    pw_device* m_device = nullptr;
    spa_hook m_listener;

    float m_volume = 0;
};

#endif // QPIPEWIREDEVICE_H
