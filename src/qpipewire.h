#pragma once

#include <QObject>

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

class QPipewireMetadata;
class QPipewireSettings;
class QPipewireClient;

#include "src/qpipewiremetadata.h"
#include "src/qpipewiresettings.h"

class QPipewire : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPipewireSettings* settings READ settings)
    Q_PROPERTY(QPipewireClient* client READ client)

signals:
    void registryObject(uint32_t id,
                        uint32_t permissions,
                        const char *type,
                        uint32_t version,
                        const struct spa_dict *props);

private:
    struct pw_main_loop *loop = nullptr;

    struct pw_context *context = nullptr;

    struct pw_core *core = nullptr;
    struct spa_hook core_listener;

    struct pw_registry *registry = nullptr;
    struct spa_hook registry_listener;

    int sync = 0;
    bool round_trip_done = false;

    QPipewireClient *pw_client = nullptr;
    QPipewireSettings *pw_settings = nullptr;

public:
    explicit QPipewire(int *argc, char **argv[], QObject *parent = nullptr);
    virtual ~QPipewire();

    /**
     * Executes pipewire main loop, calling callbacks and other stuff.
     * Runs in main thread and is blocking until all operations are done.
     */
    Q_INVOKABLE void round_trip();

    QPipewireClient *client() { return pw_client; }
    QPipewireSettings *settings() { return pw_settings; }

public: // actually, private
    // PIPEWIRES CALLBACKS (private)
    void _loop_quit();
    void _on_core_done(uint32_t id, int seq);
    void _on_core_error(uint32_t id, int seq, int res, const char *message);
    void _registry_event(uint32_t id,
                         uint32_t permissions,
                         const char *type,
                         uint32_t version,
                         const struct spa_dict *props);

private:
    void resync();

    friend class QPipewireClient;
    friend class QPipewireMetadata;
};
