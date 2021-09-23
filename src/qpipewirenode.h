#pragma once

#include <QObject>
#include <QIcon>

//#include <spa/utils/result.h>
//#include <spa/utils/string.h>
//#include <spa/pod/parser.h>
#include <spa/utils/defs.h>
#include <spa/debug/pod.h>

//#include <pipewire/impl.h>
//#include <pipewire/extensions/profiler.h>

class QPipewire;
class QPipewireProfiler;

class QPipewireNode : public QObject
{
    Q_OBJECT
    // S   ID  QUANT   RATE    WAIT    BUSY   W/Q   B/Q  ERR  NAME
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QPipewireNode* driver READ driver NOTIFY driverChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(double waiting READ waiting NOTIFY waitingChanged)
    Q_PROPERTY(double busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(int quantum READ quantum NOTIFY quantumChanged)
    Q_PROPERTY(int rate READ rate NOTIFY rateChanged)
    Q_PROPERTY(int error READ error NOTIFY errorChanged)
    Q_PROPERTY(int xrun READ xrun NOTIFY xrunChanged)

signals:
    void idChanged();
    void nameChanged();
    void driverChanged();
    void activeChanged();
    void waitingChanged();
    void busyChanged();
    void quantumChanged();
    void rateChanged();
    void errorChanged();
    void xrunChanged();

private:
    QPipewire *pipewire = nullptr;

    uint32_t m_id = 0;
    QString m_name;

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

public:
    QPipewireNode() = default;
    explicit QPipewireNode(QPipewire *parent, uint32_t id, const struct spa_dict *props);
    virtual ~QPipewireNode();

    int id() const { return m_id; }
    QString name() const { return m_name; }
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

    Q_INVOKABLE QString formatPercentage(float val, float quantum) const;
    Q_INVOKABLE QIcon activeIcon(bool active) const;


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

    friend class QPipewireProfiler;
};

Q_DECLARE_METATYPE(QPipewireNode*);
