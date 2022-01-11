#include "qpipewirelink.h"
#include "src/pw/qpipewire.h"
#include "src/pw/utils.h"

QPipewireLink::QPipewireLink(QPipewire* parent, uint32_t id, const spa_dict* props)
    : QObject(parent)
    , pipewire(parent)
    , m_id(id)
    , m_input_port(spa_dict_get_u32(props, PW_KEY_LINK_INPUT_PORT))
    , m_output_port(spa_dict_get_u32(props, PW_KEY_LINK_OUTPUT_PORT))
    , m_input_node(spa_dict_get_u32(props, PW_KEY_LINK_INPUT_NODE))
    , m_output_node(spa_dict_get_u32(props, PW_KEY_LINK_INPUT_NODE))
{
//     qDebug() << "Adding link (" << id << ") with props:";
//     const struct spa_dict_item *item;
//     spa_dict_for_each(item, props) {
//         qDebug() << '\t' << item->key << ":" << item->value;
//     }
}

QPipewireLink::~QPipewireLink()
{
}

