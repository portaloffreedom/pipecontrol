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
#include <spa/utils/dict.h>
#include "qpipewireport.h"
#include "qpipewire.h"

#define QPIPEWIRE_CAST(x) QPipewirePort* _this = static_cast<QPipewirePort*>(x);

static void static_port_info(void *data, const struct pw_port_info *info)
{
	QPIPEWIRE_CAST(data);
	_this->_port_info(info);
}
static void static_port_param(void *data, int seq,
							  uint32_t id, uint32_t index, uint32_t next,
                              const struct spa_pod *param)
{
	QPIPEWIRE_CAST(data);
	_this->_port_param(seq, id, index, next, param);
}

static const pw_port_events port_events {
	.version = PW_VERSION_PORT_EVENTS,
	.info = static_port_info,
	.param = static_port_param,
};

QPipewirePort::QPipewirePort(QPipewire* parent, uint32_t id, const spa_dict* props)
	: QObject(parent)
	, pipewire(parent)
	, m_id(id)
{
	port = static_cast<pw_port*>(
			pw_registry_bind(pipewire->registry, id, PW_TYPE_INTERFACE_Port, PW_VERSION_CLIENT, 0));

	if (port == nullptr) {
		throw std::runtime_error("Error creating port proxy");
	}

	pw_port_add_listener(port,
						 &port_listener,
						 &port_events,
						 this);
//     qDebug() << "Adding port (" << id << ") with props:";
//     const struct spa_dict_item *item;
//     spa_dict_for_each(item, props) {
//         qDebug() << '\t' << item->key << ":" << item->value;
//     }
}

QPipewirePort::~QPipewirePort()
{
	spa_hook_remove(&port_listener);
	if (port != nullptr) {
		pw_proxy_destroy((struct pw_proxy*) port);
	}
}

uint32_t QPipewirePort::id() const
{
    return m_id;
}

void QPipewirePort::set_id(uint32_t id)
{
    if (m_id == id) {
        return;
    }

    m_id = id;
    emit idChanged(m_id);
}

void QPipewirePort::_port_info(const struct pw_port_info *info)
{
  //    qDebug() << "Port info (" << this->m_id << "):";
  //    qDebug() << "\t change_mask:" << info->change_mask;
  //    qDebug() << "\t direction:" << info->direction;
  //    qDebug() << "\t id:" << info->id;
  //    qDebug() << "\t n_params:" << info->n_params;
	 // for(uint32_t i=0; i<info->n_params; i++) {
		//  const spa_param_info* param = info->params+i;
		//  qDebug() << "\t params("<<i<<')';
		//  qDebug() << "\t\tid:" << param->id;
		//  qDebug() << "\t\tflags:" << param->flags;
		//  qDebug() << "\t\tuser:" << param->user;
	 // }
  //    qDebug() << "\t params_id:" << info->params->id;
  //    qDebug() << "\t params_flags:" << info->params->flags;
  //    qDebug() << "\t params_user:" << info->params->user;
  //    qDebug() << "\t props:" << info->props;
  //    const struct spa_dict_item *item;
  //    spa_dict_for_each(item, info->props) {
  //        qDebug() << "\t\t" << item->key << ":" << item->value;
  //    }
}

void QPipewirePort::_port_param(int seq, uint32_t id, uint32_t index, uint32_t next, const struct spa_pod *param)
{
	// qDebug() << "Port param (" << this->m_id << "):";
	// qDebug() << "\t seq:" << seq;
	// qDebug() << "\t id:" << id;
	// qDebug() << "\t index:" << index;
	// qDebug() << "\t next:" << next;
	// qDebug() << "\t param:" << param;
}
