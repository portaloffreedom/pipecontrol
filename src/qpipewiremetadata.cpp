#include "qpipewiremetadata.h"

#include <QDebug>
#include "src/qpipewire.h"

#define QPIPEWIRE_CAST(x) QPipewireMetadata* _this = static_cast<QPipewireMetadata*>(x);

//-----------------------------------------------------------------------------
static int metadata_property(void *data,
                             uint32_t id,
                             const char* key,
                             const char* type,
                             const char* value)
{
    QPIPEWIRE_CAST(data);
    return _this->_metadata_property(id, key, type, value);
}

int QPipewireMetadata::_metadata_property(uint32_t id,
                                          const char* key,
                                          const char* type,
                                          const char* value)
{
    if (key == nullptr) {
        emit onAllKeysRemoved(id);
    } else if (value == nullptr) {
        emit onKeyRemoved(id, key);
    } else {
        emit onKeyUpdated(id, key, type, value);
    }

    return 0;
}

static const pw_metadata_events metadata_events {
    .version = PW_VERSION_METADATA_EVENTS,
    .property = metadata_property,
};

//-----------------------------------------------------------------------------

QPipewireMetadata::QPipewireMetadata(QPipewire *parent, uint32_t id, const char *type)
    : QObject(parent)
    , pipewire(parent)
{
    metadata = static_cast<pw_metadata*>(
                   pw_registry_bind(parent->registry, id, type, PW_VERSION_METADATA, 0));
    pw_metadata_add_listener(metadata,
                             &metadata_listener,
                             &metadata_events,
                             this);
    parent->resync();
}

QPipewireMetadata::~QPipewireMetadata()
{
    spa_hook_remove(&metadata_listener);
    if (metadata != nullptr) {
        pw_proxy_destroy((struct pw_proxy*) metadata);
    }
}

//-----------------------------------------------------------------------------

void QPipewireMetadata::setProperty(const char *key, const char *value)
{
    pw_metadata_set_property(metadata,
                             0,
                             key,
                             nullptr,
                             value);
    pipewire->round_trip();
}
