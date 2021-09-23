#include "qpipewirenode.h"
#include "src/qpipewire.h"
#include <QtDebug>

QPipewireNode::QPipewireNode(QPipewire *parent, uint32_t id, const struct spa_dict *props)
    : QObject(parent)
    , pipewire(parent)
{
    const char* str;
    if ((str = spa_dict_lookup(props, PW_KEY_NODE_NAME)) == NULL &&
        (str = spa_dict_lookup(props, PW_KEY_NODE_DESCRIPTION)) == NULL)
    {
        str = spa_dict_lookup(props, PW_KEY_APP_NAME);
    }

    if (str == nullptr) {
        m_name = QString(id);
    } else {
        m_name = str;
    }

    m_id = id;
    m_driver = this;
}

QPipewireNode::~QPipewireNode()
{

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
