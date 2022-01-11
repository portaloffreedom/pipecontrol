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
#include "qpipewireport.h"

QPipewirePort::QPipewirePort()
{
}

QPipewirePort::~QPipewirePort()
{
}

uint32_t QPipewirePort::m_id() const
{
    return m_m_id;
}

void QPipewirePort::setM_id(uint32_t m_id)
{
    if (m_m_id == m_id) {
        return;
    }

    m_m_id = m_id;
    emit m_idChanged(m_m_id);
}
