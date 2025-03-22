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
#include "qpipewirenode.h"
#include "src/pw/qpipewire.h"
#include "src/pw/utils.h"
#include <QtDebug>
#include <spa/param/props.h>
#include <spa/param/param.h>
#include <spa/pod/vararg.h>
#include <spa/pod/builder.h>
#include <sstream>
#include <stdexcept>
#include <iostream>

template <typename T>
T clamp(const T v, const T low, const T high)
{
    if (v < low)
        return low;
    if (v > high)
        return high;
    return v;
}

#define QPIPEWIRE_CAST(x) QPipewireNode* _this = static_cast<QPipewireNode*>(x);

void node_event_info(void *data, const pw_node_info *info)
{
    QPIPEWIRE_CAST(data);
    _this->_node_event_info(info);
}

void QPipewireNode::_node_event_info(const pw_node_info* info)
{
    // message when it's updated
    // qDebug() << "node_event_info: ";
    const struct spa_dict_item *item;

    if (info->change_mask == PW_NODE_CHANGE_MASK_ALL) {
        //TODO
        // qDebug() << "CHANGE ALL";
    }
    if (info->change_mask & PW_NODE_CHANGE_MASK_INPUT_PORTS ) {
        //TODO
        // qDebug() << "CHANGE INPUT PORTS";
    }
    if (info->change_mask & PW_NODE_CHANGE_MASK_OUTPUT_PORTS) {
        //TODO
        // qDebug() << "CHANGE OUTPUT PORTS";
    }
    if (info->change_mask & PW_NODE_CHANGE_MASK_STATE) {
        m_state = info->state;
        Q_EMIT stateChanged();
        // qDebug() << "CHANGE STATE";
    }
    if (info->change_mask & PW_NODE_CHANGE_MASK_PROPS) {
        spa_dict_for_each(item, info->props) {
            //volume is not here....
            const QString key = QString::fromUtf8(item->key);
            const QString value = QString::fromUtf8(item->value);
            m_properties[key] = value;
            //qWarning() << "node_" << this->m_name << ":[" << key << "]=" << value;
            //Q_EMIT propertyChanged(key, value);
        }
        // qDebug() << "CHANGE PROPS";
    }
    if (info->change_mask & PW_NODE_CHANGE_MASK_PARAMS) {
        enumParams();
        // qDebug() << "CHANGE PARAMS";
    }

    // spa_dict_for_each(item, info->props) {
    //     //volume is not here....
    //     const QString key = item->key;
    //     const QString value = item->value;
    //     // m_properties[key] = value;
    //     qWarning() << "node_" << this->m_name << ":[" << key << "]=" << value;
    //     //Q_EMIT propertyChanged(key, value);
    // }

    // for(uint32_t i=0; i<info->n_params; i++) {
    //     spa_param_info *param = info->params+i;
    //     QString id;
    //     switch ((spa_param_type) param->id) {
    //         case SPA_PARAM_Invalid: id = "SPA_PARAM_Invalid"; break;		/**< invalid */
    //         case SPA_PARAM_PropInfo: id = "SPA_PARAM_PropInfo"; break;		/**< property information as SPA_TYPE_OBJECT_PropInfo */
    //         case SPA_PARAM_Props: id = "SPA_PARAM_Props"; break;		/**< properties as SPA_TYPE_OBJECT_Props */
    //         case SPA_PARAM_EnumFormat: id = "SPA_PARAM_EnumFormat"; break;		/**< available formats as SPA_TYPE_OBJECT_Format */
    //         case SPA_PARAM_Format: id = "SPA_PARAM_Format"; break;		/**< configured format as SPA_TYPE_OBJECT_Format */
    //         case SPA_PARAM_Buffers: id = "SPA_PARAM_Buffers"; break;		/**< buffer configurations as SPA_TYPE_OBJECT_ParamBuffers*/
    //         case SPA_PARAM_Meta: id = "SPA_PARAM_Meta"; break;			/**< allowed metadata for buffers as SPA_TYPE_OBJECT_ParamMeta*/
    //         case SPA_PARAM_IO: id = "SPA_PARAM_IO"; break;			/**< configurable IO areas as SPA_TYPE_OBJECT_ParamIO */
    //         case SPA_PARAM_EnumProfile: id = "SPA_PARAM_EnumProfile"; break;		/**< profile enumeration as SPA_TYPE_OBJECT_ParamProfile */
    //         case SPA_PARAM_Profile: id = "SPA_PARAM_Profile"; break;		/**< profile configuration as SPA_TYPE_OBJECT_ParamProfile */
    //         case SPA_PARAM_EnumPortConfig: id = "SPA_PARAM_EnumPortConfig"; break;	/**< port configuration enumeration as SPA_TYPE_OBJECT_ParamPortConfig */
    //         case SPA_PARAM_PortConfig: id = "SPA_PARAM_PortConfig"; break;		/**< port configuration as SPA_TYPE_OBJECT_ParamPortConfig */
    //         case SPA_PARAM_EnumRoute: id = "SPA_PARAM_EnumRoute"; break;		/**< routing enumeration as SPA_TYPE_OBJECT_ParamRoute */
    //         case SPA_PARAM_Route: id = "SPA_PARAM_Route"; break;		/**< routing configuration as SPA_TYPE_OBJECT_ParamRoute */
    //         case SPA_PARAM_Control: id = "SPA_PARAM_Control"; break;		/**< Control parameter, a SPA_TYPE_Sequence */
    //         case SPA_PARAM_Latency: id = "SPA_PARAM_Latency"; break;		/**< latency reporting, a SPA_TYPE_OBJECT_ParamLatency */
    //         case SPA_PARAM_ProcessLatency: id = "SPA_PARAM_ProcessLatency"; break;	/**< processing latency, a SPA_TYPE_OBJECT_ParamProcessLatency */
    //         default: id = "ERROR"; break;
    //     }
    //     QString flags;
    //     if (param->flags & SPA_PARAM_INFO_SERIAL) {
    //         /*< bit to signal update even when the read/write flags don't change */
    //         flags += "SERIAL";
    //     }
    //     if (param->flags & SPA_PARAM_INFO_READ) {
    //         flags += "_READ";
    //     }
    //     if (param->flags & SPA_PARAM_INFO_WRITE) {
    //         flags += "_WRITE";
    //     }
    //
    //     qWarning() << "id: " << id << " -> " << flags << " <- " << param->user;
    // }

    // spa_debug_pod(2, NULL, info->props);
    // spa_debug_pod(2, NULL, info->param);

}


void event_param(void *data,
                 int seq,
                 uint32_t id,
                 uint32_t index,
                 uint32_t next,
                 const spa_pod *param)
{
    QPIPEWIRE_CAST(data);
    _this->_event_param(seq, id, index, next, param);

    //volume is here!
}

void QPipewireNode::_event_param(int seq, uint32_t id, uint32_t index, uint32_t next, const spa_pod* param)
{
    // spa_debug_pod(2, nullptr, param);

    const spa_pod_prop *prop;
    const spa_pod_object *obj = (const struct spa_pod_object*)param;
    // SPA_POD_OBJECT_FOREACH(obj, prop) {
    //     printf("prop key:%d\n", prop->key);
    // }

    prop = spa_pod_find_prop(param, nullptr, SPA_PROP_channelVolumes);
    if (prop != nullptr) {
        assert(spa_pod_is_array(&prop->value));
        const spa_pod_array *volumes = (const spa_pod_array*) &prop->value;

        uint32_t n_channels = 0;
        uint32_t type = SPA_POD_ARRAY_VALUE_TYPE(volumes);
        assert(SPA_TYPE_Float == type);
        float *data = (float*) spa_pod_get_array((const spa_pod*) volumes, &n_channels);
        m_volume = data[0];
        Q_EMIT volumeChanged(m_volume);
    }
}


static const struct pw_node_events node_events = {
    .version = PW_VERSION_NODE_EVENTS,
    .info = &node_event_info,
    .param = &event_param
};


QPipewireNode::QPipewireNode(QPipewire *parent, uint32_t id, const struct spa_dict *props)
    : QObject(parent)
    , pipewire(parent)
    , m_id(id)
    , m_node_type(NodeTypeNone)
    , m_media_type(MediaTypeNone)
    , m_driver(this)

{
    m_node_name = QString::fromUtf8(spa_dict_lookup(props, PW_KEY_NODE_NAME));
    m_node_description = QString::fromUtf8(spa_dict_lookup(props, PW_KEY_NODE_DESCRIPTION));

    // Find name
    const char* str;
    if ((str = spa_dict_lookup(props, PW_KEY_NODE_NICK)) == nullptr &&
        (str = spa_dict_lookup(props, PW_KEY_NODE_DESCRIPTION)) == nullptr &&
        (str = spa_dict_lookup(props, PW_KEY_NODE_NAME)) == nullptr)
    {
        str = spa_dict_lookup(props, PW_KEY_APP_NAME);
    }

    if (str == nullptr) {
        m_name = QString::number(id);
    } else {
        m_name = QString::fromUtf8(str);
    }

    // Find node type
    if ((str = spa_dict_lookup(props, PW_KEY_MEDIA_CATEGORY)) != nullptr) {
        m_category = QString::fromUtf8(str);
    } else {
        m_category = QStringLiteral("");
    }

    // Find node type
    if ((str = spa_dict_lookup(props, PW_KEY_MEDIA_CLASS)) != nullptr) {
        m_media_class = QString::fromUtf8(str);

        if (m_media_class.contains(QStringLiteral("Audio")))
            m_media_type = MediaTypeAudio;
        else if (m_media_class.contains(QStringLiteral("Video")))
            m_media_type = MediaTypeVideo;
        else if (m_media_class.contains(QStringLiteral("Midi")))
            m_media_type = MediaTypeMidi;
        else
            m_media_type = MediaTypeNone;
    } else {
        m_media_class = QStringLiteral("");
    }

    if ((!m_category.isEmpty()) && m_category.contains(QStringLiteral("Duplex"))) {
        m_node_type = NodeTypeNone;
    } else if (!m_media_class.isEmpty()) {
        if (m_media_class.contains(QStringLiteral("Sink"))) {
            m_node_type = NodeTypeSink;
        } else if (m_media_class.contains(QStringLiteral("Input"))) {
            m_node_type = NodeTypeInput;
        } else if (m_media_class.contains(QStringLiteral("Source"))) {
            m_node_type = NodeTypeSource;
        } else if (m_media_class.contains(QStringLiteral("Output"))) {
            m_node_type = NodeTypeOutput;
        }
    } else {
        m_node_type = NodeTypeNone;
    }

    m_pw_node = static_cast<pw_node*>(
        pw_registry_bind(pipewire->registry, m_id, PW_TYPE_INTERFACE_Node, PW_VERSION_NODE, 0));

    const struct spa_dict_item *item;
    spa_dict_for_each(item, props) {
        // qDebug() << "Property[" << item->key << "] =" << item->value;
        // std::cout << "Property[" << item->key << "]=" << item->value << std::endl;
        m_properties[QString::fromUtf8(item->key)] = QString::fromUtf8(item->value);
    }

//    struct spa_pod_object *obj = (struct spa_pod_object *) m_pw_node;
//    struct spa_pod_prop *prop;
//    SPA_POD_OBJECT_FOREACH(obj, prop) {
//        switch (prop->key) {
//        case SPA_PROP_volume:
//            float volume;
//            if (spa_pod_get_float(&prop->value, &volume) < 0)
//                continue;
//            if (volume == m_volume)
//                continue;
//            m_volume = volume;
//            Q_EMIT volumeChanged(m_volume);
//            break;
//        }
//        const spa_pod *pod = &prop->value;
//        std::cout << "KEY: " << prop->key << "=" << prop->value << std::endl;
//    }
    pw_proxy_add_object_listener((pw_proxy*) this->m_pw_node, &object_listener, &node_events, this);
    // enumParams();
}

QPipewireNode::~QPipewireNode()
{
    if(m_pw_node != nullptr) {
        pw_proxy_destroy((pw_proxy *) m_pw_node);
    }
}

QString QPipewireNode::formatPercentage(float val, float quantum) const
{
    QString buf;
    buf.asprintf("%5.2f", quantum == 0.0f ? 0.0f : val/quantum);
    return buf;
}

QIcon QPipewireNode::activeIcon(bool active) const
{
    if(active) {
        return QIcon::fromTheme(QStringLiteral("media-playback-start"));
    } else {
        return QIcon::fromTheme(QStringLiteral("media-playback-pause"));
    }
}

void QPipewireNode::setDriver(QPipewireNode* newDriver)
{
    if (newDriver == m_driver) return;
    m_driver = newDriver;
    Q_EMIT driverChanged();
}

void QPipewireNode::setMeasurement(const struct QPipewireNode::measurement &measure)
{
    const struct measurement old = this->measurement;
    this->measurement = measure;
    if (old.status != measure.status) Q_EMIT activeChanged();
    if (old.signal != measure.signal || old.awake != measure.awake)
        Q_EMIT waitingChanged();
    if (old.finish != measure.finish || old.awake != measure.awake)
        Q_EMIT busyChanged();
    if (m_driver != this && old.latency.num != measure.latency.num)
        Q_EMIT quantumChanged();
    if (m_driver != this && old.latency.denom != measure.latency.denom)
        Q_EMIT rateChanged();
}

void QPipewireNode::setInfo(const struct QPipewireNode::driver &info)
{
    const struct driver old = this->info;
    this->info = info;
    if (m_driver == this &&
        (old.clock.duration != info.clock.duration || old.clock.rate.num != info.clock.rate.num))
        Q_EMIT quantumChanged();
    if (m_driver == this && old.clock.rate.denom != info.clock.rate.denom)
        Q_EMIT rateChanged();
    if (old.xrun_count != info.xrun_count)
        Q_EMIT xrunChanged();
}

QVariant QPipewireNode::property(const char *key)
{
    return m_properties[QString::fromUtf8(key)];
}

void QPipewireNode::setProperty(const char *key, QVariant value)
{
    throw std::runtime_error("not implemented");
}

void QPipewireNode::setProperties(struct spa_pod *properties)
{
    spa_debug_pod(0, nullptr, properties);
    int res = pw_node_set_param(m_pw_node, SPA_PARAM_Props, 0, properties);
    if (res < 0) {
        std::ostringstream err_msg;
        err_msg << "Got set_property error \"" << res << '"';
        throw std::runtime_error(err_msg.str());
    }
}

void QPipewireNode::setVolume(float volume)
{
    volume = clamp(volume, 0.0f, 1.0f);
    struct spa_pod_builder builder = {};
    unsigned char buffer[512];
    spa_pod_builder_init(&builder, buffer, sizeof(buffer));

    struct spa_pod *props = static_cast<spa_pod*>(
        spa_pod_builder_add_object(&builder,
                                   SPA_TYPE_OBJECT_Props, 0,
                                   SPA_PROP_volume, SPA_POD_Float(volume)));


    this->setProperties(props);

    m_volume = volume;
    Q_EMIT volumeChanged(m_volume);
}


void QPipewireNode::enumParams()
// static bool do_enum_params(struct data *data, const char *cmd, char *args, char **error)
{
    spa_pod *filter = nullptr;
    pw_node_enum_params(m_pw_node, _props_seq++, SPA_PARAM_Props, 0, 0, filter);
    // pw_node_enum_params(m_pw_node, _props_seq++, SPA_PARAM_Route, 0, 0, filter);
    // pw_node_set_param();
}

