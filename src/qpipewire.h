#pragma once

#include <QObject>
#include <QList>
#include <QVariantList>

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

class QPipewireMetadata;
class QPipewireSettings;
class QPipewireClient;

#include "src/qpipewiremetadata.h"
#include "src/qpipewiresettings.h"
#include "src/qpipewirenode.h"
#include "src/qpipewireprofiler.h"
#include "src/qpipewirenodelistmodel.h"

class QPipewire : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPipewireSettings* settings READ settings NOTIFY settingsChanged)
    Q_PROPERTY(QPipewireClient* client READ client NOTIFY clientChanged)
    Q_PROPERTY(QPipewireNodeListModel* nodes READ nodes NOTIFY nodesChanged)
    Q_PROPERTY(QList<QPipewireNode*> nodeList READ nodeList NOTIFY nodesChanged)
    Q_PROPERTY(QPipewireProfiler* profiler READ profiler NOTIFY profilerChanged)

signals:
    void quit();
    void settingsChanged();
    void clientChanged();
    void nodesChanged();
    void profilerChanged();
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
    QPipewireNodeListModel *m_nodes = nullptr;
    QPipewireProfiler *pw_profiler = nullptr;

public:
    explicit QPipewire(int *argc, char **argv[], QObject *parent = nullptr);
    virtual ~QPipewire();

    /**
     * Executes pipewire main loop, calling callbacks and other stuff.
     * Runs in main thread and is blocking until all operations are done.
     */
    Q_INVOKABLE void round_trip();

    QPipewireClient* client() { return pw_client; }
    QPipewireSettings* settings() { return pw_settings; }
    QPipewireProfiler* profiler() { return pw_profiler; }
    QPipewireNodeListModel* nodes() { return m_nodes; }
    QList<QPipewireNode*> nodeList() { return m_nodes->list(); }
    QObjectList nodeObjectList() {
        auto list = QObjectList();
        for(int i=0; i<m_nodes->size(); i++) {
            list.append(m_nodes->list()[i]);
        }
        return list;
    }

public: // actually, private
    // PIPEWIRES CALLBACKS (private)
    void _quit();
    void _on_core_done(uint32_t id, int seq);
    void _on_core_error(uint32_t id, int seq, int res, const char *message);
    void _registry_event(uint32_t id,
                         uint32_t permissions,
                         const char *type,
                         uint32_t version,
                         const struct spa_dict *props);
    void _registry_event_remove(uint32_t id);

private:
    void resync();

    friend class QPipewireClient;
    friend class QPipewireMetadata;
    friend class QPipewireProfiler;
    friend class QPipewireNode;
};