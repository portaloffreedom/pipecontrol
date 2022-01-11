#ifndef QPIPEWIREPORT_H
#define QPIPEWIREPORT_H

#include <qobject.h>

/**
 * @todo write docs
 */
class QPipewirePort : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint32_t m_id READ m_id WRITE setM_id NOTIFY m_idChanged)

public:
    /**
     * Default constructor
     */
    QPipewirePort();

    /**
     * Destructor
     */
    ~QPipewirePort();

    /**
     * @return the m_id
     */
    uint32_t m_id() const;

public Q_SLOTS:
    /**
     * Sets the m_id.
     *
     * @param m_id the new m_id
     */
    void setM_id(uint32_t m_id);

Q_SIGNALS:
    void m_idChanged(uint32_t m_id);

private:
    uint32_t m_m_id;
};

#endif // QPIPEWIREPORT_H
