#ifndef QPIPEWIRELINK_H
#define QPIPEWIRELINK_H

#include <qobject.h>
#include <spa/utils/dict.h>

class QPipewire;

/**
 * @todo write docs
 */
class QPipewireLink : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(int inputPort READ inputPort NOTIFY inputPortChanged)
    Q_PROPERTY(int outputPort READ outputPort NOTIFY outputPortChanged)
    Q_PROPERTY(int inputNode READ inputNode NOTIFY inputNodeChanged)
    Q_PROPERTY(int outputNode READ outputNode NOTIFY outputNodeChanged)


signals:
    void idChanged();
    void inputPortChanged();
    void outputPortChanged();
    void inputNodeChanged();
    void outputNodeChanged();

protected:
    QPipewire* pipewire;
    uint32_t m_id;
    uint32_t m_input_port;
    uint32_t m_output_port;
    uint32_t m_input_node;
    uint32_t m_output_node;

public:
    /**
     * Default constructor
     */
    QPipewireLink() = default;
    QPipewireLink(QPipewire* parent, uint32_t id, const spa_dict* props);

    /**
     * Destructor
     */
    virtual ~QPipewireLink();

    int id() const { return m_id; }
    uint32_t id_u32() const { return m_id; }
    uint32_t inputPort() const  { return m_input_port; }
    uint32_t outputPort() const { return m_output_port; }
    uint32_t inputNode() const  { return m_input_node; }
    uint32_t outputNode() const { return m_output_node; }
};

std::ostream& operator<< (std::ostream& out, const QPipewireLink& link);

#endif // QPIPEWIRELINK_H
