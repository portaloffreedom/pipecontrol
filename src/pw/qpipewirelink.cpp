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
#include "qpipewirelink.h"
#include "src/pw/qpipewire.h"
#include "src/pw/utils.h"

QPipewireLink::QPipewireLink(QPipewire* parent, uint32_t id, const spa_dict* props)
    : QObject(parent)
    , pipewire(parent)
    , m_id(id)
    , m_input_port(spa_dict_get_u32(props, PW_KEY_LINK_INPUT_PORT))
    , m_output_port(spa_dict_get_u32(props, PW_KEY_LINK_OUTPUT_PORT))
    , m_input_node(spa_dict_get_u32(props, PW_KEY_LINK_INPUT_NODE))
    , m_output_node(spa_dict_get_u32(props, PW_KEY_LINK_INPUT_NODE))
{
	link = static_cast<pw_link*>(
			pw_registry_bind(pipewire->registry, id, PW_TYPE_INTERFACE_Link, PW_VERSION_CLIENT, 0));

	if (link == nullptr) {
		throw std::runtime_error("Error creating link proxy");
	}
//     qDebug() << "Adding link (" << id << ") with props:";
//     const struct spa_dict_item *item;
//     spa_dict_for_each(item, props) {
//         qDebug() << '\t' << item->key << ":" << item->value;
//     }
}

QPipewireLink::~QPipewireLink()
{
	if (link != nullptr) {
		pw_proxy_destroy((struct pw_proxy*) link);
	}
}

