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
#include <pipewire/keys.h>
#include "qpipewirealsanode.h"

QPipewireAlsaNode::QPipewireAlsaNode(QPipewire *parent, uint32_t id, const struct spa_dict *props)
        : QPipewireNode(parent, id, props)
{
    m_name = spa_dict_lookup(props, PW_KEY_NODE_NICK);
}
