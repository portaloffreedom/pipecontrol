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
