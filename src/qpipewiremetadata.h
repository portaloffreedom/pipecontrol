#pragma once

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

#include <QObject>

class QPipewire;

class QPipewireMetadata : public QObject
{
    Q_OBJECT
signals:
    void onAllKeysRemoved(uint32_t id);
    void onKeyRemoved(uint32_t id, const char* key);
    void onKeyUpdated(uint32_t id, const char* key, const char* type, const char* value);


protected:
    QPipewire *pipewire = nullptr;
    struct pw_metadata *metadata = nullptr;
    struct spa_hook metadata_listener;

public:
    explicit QPipewireMetadata(QPipewire *parent, uint32_t id, const char *type);
    virtual ~QPipewireMetadata();

    void setProperty(const char *key, const char *value);
    void clear();

    int _metadata_property(uint32_t id,
                           const char* key,
                           const char* type,
                           const char* value);
};
