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


QString QPipewireNode::formatTime(double val) const
{
    val *= 1000000000.0f;
    QString buf;
    if (val < 1000000llu) {
        val /= 1000.0f;
        buf = QString::asprintf("%5.1fµs", val);
    } else if (val < 1000000000llu) {
        val /= 1000000.0f;
        buf = QString::asprintf("%5.1fms", val);
    } else {
        val /= 1000000000.0f;
        buf = QString::asprintf("%5.1fs", val);
    }
    return buf;
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
