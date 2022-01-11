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

#include <QObject>
#include <QIcon>
#include <QtQml>

//#include <spa/utils/result.h>
//#include <spa/utils/string.h>
//#include <spa/pod/parser.h>
#include <spa/utils/defs.h>
#include <spa/debug/pod.h>
#include <spa/node/node.h>

//#include <pipewire/impl.h>
//#include <pipewire/extensions/profiler.h>

class QPipewire;
class QPipewireProfiler;

class QPipewireNode : public QObject
{
public:
    enum NodeType {
        NodeTypeNone = 0,
        NodeTypeInput,
        NodeTypeOutput,
        NodeTypeSink,
        NodeTypeSource,
    };

    enum MediaType {
        MediaTypeNone = 0,
        MediaTypeAudio,
        MediaTypeVideo,
        MediaTypeMidi,
    };

Q_OBJECT
    Q_ENUM(NodeType);
    Q_ENUM(MediaType);
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString category READ category NOTIFY categoryChanged)
    Q_PROPERTY(QString mediaClass READ mediaClass NOTIFY mediaClassChanged)
    Q_PROPERTY(NodeType nodeType READ nodeType NOTIFY nodeTypeChanged)
    Q_PROPERTY(MediaType mediaType READ mediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY(QPipewireNode* driver READ driver NOTIFY driverChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(double waiting READ waiting NOTIFY waitingChanged)
    Q_PROPERTY(double busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(int quantum READ quantum NOTIFY quantumChanged)
    Q_PROPERTY(int rate READ rate NOTIFY rateChanged)
    Q_PROPERTY(int error READ error NOTIFY errorChanged)
    Q_PROPERTY(int xrun READ xrun NOTIFY xrunChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)

signals:
    void idChanged();
    void nameChanged();
    void categoryChanged();
    void mediaClassChanged();
    void nodeTypeChanged();
    void mediaTypeChanged();
    void driverChanged();
    void activeChanged();
    void waitingChanged();
    void busyChanged();
    void quantumChanged();
    void rateChanged();
    void errorChanged();
    void xrunChanged();
    void volumeChanged(float);

private:
    QPipewire *pipewire = nullptr;

    uint32_t m_id = 0;
    QString m_name;
    QString m_category;
    QString m_media_class;
    NodeType m_node_type = NodeTypeNone;
    MediaType m_media_type = MediaTypeNone;
    struct spa_node *m_spa_node = nullptr;
    struct spa_node_info m_spa_node_info {};

    struct measurement {
            int32_t index = 0;
            int32_t status = 0;
            int64_t quantum = 0;
            int64_t prev_signal = 0;
            int64_t signal = 0;
            int64_t awake = 0;
            int64_t finish = 0;
            struct spa_fraction latency = {0,0};
    } measurement;

    struct driver {
            int64_t count = 0;
            float cpu_load[3] = {0,0,0};
            struct spa_io_clock clock = {0,0,"",0,{0,0},0,0,0,0,0,{0,0,0,0,0,0,0,0}};
            uint32_t xrun_count = 0;
    } info;

    QPipewireNode *m_driver = nullptr;
    uint32_t errors = 0;
    int32_t last_error_status = 0;

    float m_volume = 0.0;

public:
    QPipewireNode() = default;
    explicit QPipewireNode(QPipewire *parent, uint32_t id, const struct spa_dict *props);
    virtual ~QPipewireNode();

    int id() const { return m_id; }
    uint32_t id_u32() const { return m_id; }
    QString name() const { return m_name; }
    QString category() const { return m_category; }
    QString mediaClass() const { return m_media_class; }
    NodeType nodeType() const { return m_node_type; }
    MediaType mediaType() const { return m_media_type; }
    QPipewireNode *driver() { return m_driver != this ? m_driver : nullptr; }
    bool active() const { return measurement.status == 3; }
    double waiting() const { return (measurement.awake - measurement.signal) / 1000000000.f; }
    double busy() const { return (measurement.finish - measurement.awake) / 1000000000.f; }
    int quantum() const {
        if (m_driver == this) {
            return info.clock.duration * info.clock.rate.num;
        } else {
            return measurement.latency.num;
        }
    }
    int rate() const {
        if (m_driver == this) {
            return info.clock.rate.denom;
        } else {
            return measurement.latency.denom;
        }
    }
    int error() const { return errors; }
    int xrun() const { return info.xrun_count; }
    float volume() const { return m_volume; }

    Q_INVOKABLE QString formatPercentage(float val, float quantum) const;
    Q_INVOKABLE QIcon activeIcon(bool active) const;

    /// Volume has to be between 0.0 and 1.0 (included)
    Q_INVOKABLE void setVolume(float volume);

private:

    float _quantum()
    {
        if (info.clock.rate.denom)
            return info.clock.duration * info.clock.rate.num / info.clock.rate.denom;
        else
            return 0;
    }

    void setDriver(QPipewireNode *newDriver);
    void setMeasurement(const struct measurement &measure);
    void setInfo(const struct driver &info);

    void setProperties(struct spa_pod *properties);

    friend class QPipewireProfiler;
};

Q_DECLARE_METATYPE(QPipewireNode*);
