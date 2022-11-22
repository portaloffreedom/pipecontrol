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

#include <qobject.h>
#include "qpipewire.h"

/**
 * @todo write docs
 */
class QPipewirePort : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint32_t id READ id WRITE set_id NOTIFY idChanged)

public:
    /**
     * Default constructor
     */
    QPipewirePort(QPipewire *parent, uint32_t id, const spa_dict *props);

    /**
     * Destructor
     */
    ~QPipewirePort() override;

    /**
     * @return the m_id
     */
    uint32_t id() const;


	void _port_info(const struct pw_port_info *info);

	void _port_param(int seq,
	                 uint32_t id, uint32_t index, uint32_t next,
	                 const struct spa_pod *param);

public Q_SLOTS:
    /**
     * Sets the id.
     *
     * @param id the new id
     */
    void set_id(uint32_t id);

Q_SIGNALS:
    void idChanged(uint32_t id);

private:
	QPipewire* pipewire;
    uint32_t m_id = 0;
	pw_port *port = nullptr;
	spa_hook port_listener;
};
