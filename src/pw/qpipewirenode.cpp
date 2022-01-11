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
#include <QtDebug>
#include <spa/param/props.h>
#include <spa/param/param.h>
#include <spa/pod/vararg.h>
#include <spa/pod/builder.h>
#include <sstream>
#include <stdexcept>

template <typename T>
T clamp(const T v, const T low, const T high)
{
    if (v < low)
        return low;
    if (v > high)
        return high;
    return v;
}

QPipewireNode::QPipewireNode(QPipewire *parent, uint32_t id, const struct spa_dict *props)
    : QObject(parent)
    , pipewire(parent)
    , m_id(id)
    , m_node_type(NodeTypeNone)
    , m_media_type(MediaTypeNone)
    , m_driver(this)

{
    // Find name
    const char* str;
    if ((str = spa_dict_lookup(props, PW_KEY_NODE_NAME)) == nullptr &&
        (str = spa_dict_lookup(props, PW_KEY_NODE_DESCRIPTION)) == nullptr)
    {
        str = spa_dict_lookup(props, PW_KEY_APP_NAME);
    }

    if (str == nullptr) {
        m_name = QString(id);
    } else {
        m_name = str;
        if (m_name.startsWith("alsa_")) {
            QStringList split = m_name.split('.');

            m_name = split[0].replace('_', ' ').toCaseFolded()
                   + ' '
                   + split[1].replace('_', ' ');
        }
    }

    // Find node type
    if ((str = spa_dict_lookup(props, PW_KEY_MEDIA_CATEGORY)) != nullptr) {
        m_category = str;
    } else {
        m_category = "";
    }

    // Find node type
    if ((str = spa_dict_lookup(props, PW_KEY_MEDIA_CLASS)) != nullptr) {
        m_media_class = str;

        if (m_media_class.contains("Audio"))
            m_media_type = MediaTypeAudio;
        else if (m_media_class.contains("Video"))
            m_media_type = MediaTypeVideo;
        else if (m_media_class.contains("Midi"))
            m_media_type = MediaTypeMidi;
        else
            m_media_type = MediaTypeNone;
    } else {
        m_media_class = "";
    }

    if ((!m_category.isEmpty()) && m_category.contains("Duplex")) {
        m_node_type = NodeTypeNone;
    } else if (!m_media_class.isEmpty()) {
        if (m_media_class.contains("Sink")) {
            m_node_type = NodeTypeSink;
        } else if (m_media_class.contains("Input")) {
            m_node_type = NodeTypeInput;
        } else if (m_media_class.contains("Source")) {
            m_node_type = NodeTypeSource;
        } else if (m_media_class.contains("Output")) {
            m_node_type = NodeTypeOutput;
        }
    } else {
        m_node_type = NodeTypeNone;
    }

    m_spa_node = static_cast<spa_node*>(
        pw_registry_bind(pipewire->registry, m_id, PW_TYPE_INTERFACE_Node, PW_VERSION_NODE, 0));

//    struct spa_pod_object *obj = (struct spa_pod_object *) m_spa_node;
//    struct spa_pod_prop *prop;
//    SPA_POD_OBJECT_FOREACH(obj, prop) {
//        qDebug() << "KEY: " << prop->key;
//        switch (prop->key) {
//        case SPA_PROP_volume:
//            float volume;
//            if (spa_pod_get_float(&prop->value, &volume) < 0)
//                continue;
//            if (volume == m_volume)
//                continue;
//            m_volume = volume;
//            emit volumeChanged(m_volume);
//            break;
//        }
//    }
}

QPipewireNode::~QPipewireNode()
{}

QString QPipewireNode::formatPercentage(float val, float quantum) const
{
    QString buf;
    buf.asprintf("%5.2f", quantum == 0.0f ? 0.0f : val/quantum);
    return buf;
}

QIcon QPipewireNode::activeIcon(bool active) const
{
    if(active) {
        return QIcon::fromTheme("media-playback-start");
    } else {
        return QIcon::fromTheme("media-playback-pause");
    }
}

void QPipewireNode::setDriver(QPipewireNode* newDriver)
{
    if (newDriver == m_driver) return;
    m_driver = newDriver;
    emit driverChanged();
}

void QPipewireNode::setMeasurement(const struct QPipewireNode::measurement &measure)
{
    const struct measurement old = this->measurement;
    this->measurement = measure;
    if (old.status != measure.status) emit activeChanged();
    if (old.signal != measure.signal || old.awake != measure.awake)
        emit waitingChanged();
    if (old.finish != measure.finish || old.awake != measure.awake)
        emit busyChanged();
    if (m_driver != this && old.latency.num != measure.latency.num)
        emit quantumChanged();
    if (m_driver != this && old.latency.denom != measure.latency.denom)
        emit rateChanged();
}

void QPipewireNode::setInfo(const struct QPipewireNode::driver &info)
{
    const struct driver old = this->info;
    this->info = info;
    if (m_driver == this &&
        (old.clock.duration != info.clock.duration || old.clock.rate.num != info.clock.rate.num))
        emit quantumChanged();
    if (m_driver == this && old.clock.rate.denom != info.clock.rate.denom)
        emit rateChanged();
    if (old.xrun_count != info.xrun_count)
        emit xrunChanged();
}

void QPipewireNode::setProperties(struct spa_pod *properties)
{
    spa_debug_pod(0, nullptr, properties);
    int res = pw_node_set_param(m_spa_node, SPA_PARAM_Props, 0, properties);
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
    emit volumeChanged(m_volume);
}
