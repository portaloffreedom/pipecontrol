#pragma once

#include <QObject>
#include <QList>
#include <QVariantList>
#include <QGuiApplication>

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

class QPipewireMetadata;
class QPipewireSettings;
class QPipewireLink;
class QPipewireClient;
class AlsaProperties;

#include "src/pw/qpipewiremetadata.h"
#include "src/pw/qpipewiresettings.h"
#include "src/pw/qpipewirenode.h"
#include "src/pw/qpipewireprofiler.h"
#include "src/pw/qpipewirenodelistmodel.h"
#include "src/systemdservice.h"
#include "src/alsaproperties.h"

class QPipewire : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appVersion READ appVersion NOTIFY appVersionChanged)
    Q_PROPERTY(QPipewireSettings* settings READ settings NOTIFY settingsChanged)
    Q_PROPERTY(QPipewireClient* client READ client NOTIFY clientChanged)
    Q_PROPERTY(QPipewireNodeListModel* nodes READ nodes NOTIFY nodesChanged)
    Q_PROPERTY(QList<QPipewireNode*> nodeList READ nodeList NOTIFY nodesChanged)
    Q_PROPERTY(QList<QPipewireLink*> linkList READ linkList NOTIFY linksChanged)
    Q_PROPERTY(QPipewireProfiler* profiler READ profiler NOTIFY profilerChanged)
    Q_PROPERTY(SystemdService* pipewireMediaSession READ pipewireMediaSession NOTIFY pipewireMediaSessionChanged)
    Q_PROPERTY(AlsaProperties* alsaProperties READ alsaProperties NOTIFY alsaPropertiesChanged)

signals:
    void quit();
    void registryObject(uint32_t id,
                        uint32_t permissions,
                        const char *type,
                        uint32_t version,
                        const struct spa_dict *props);

    void appVersionChanged(); //bogus, never emitted
    void settingsChanged();
    void clientChanged();
    void nodesChanged();
    void linksChanged();
    void profilerChanged();
    void pipewireMediaSessionChanged();
    void alsaPropertiesChanged();

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
    QList<QPipewireLink*> m_links;
    QPipewireProfiler *pw_profiler = nullptr;
    SystemdService *pipewire_media_session = nullptr;
    AlsaProperties *alsa_properties = nullptr;

public:
    explicit QPipewire(int *argc, char **argv[], QObject *parent = nullptr);
    virtual ~QPipewire();

    /**
     * Executes pipewire main loop, calling callbacks and other stuff.
     * Runs in main thread and is blocking until all operations are done.
     */
    Q_INVOKABLE void round_trip();

    QString appVersion() { return QString(PROJECT_VERSION); }
    Q_INVOKABLE QString pipewireCompiledVersion() { return pw_get_headers_version(); }
    Q_INVOKABLE QString pipewireLinkedVersion() { return pw_get_library_version(); }
    Q_INVOKABLE QString platformName() { return QGuiApplication::platformName(); }
    Q_INVOKABLE QString qtCompiledVersion() { return QStringLiteral(QT_VERSION_STR); }
    Q_INVOKABLE QString qtLinkedVersion() { return QString::fromLocal8Bit(qVersion()); }
    //Q_INVOKABLE QString kframeworksVersion() { return QStringLiteral(KXMLGUI_VERSION_STRING); }

    Q_INVOKABLE static QString formatTime(double val);

    QPipewireClient* client() { return pw_client; }
    QPipewireSettings* settings() { return pw_settings; }
    QPipewireProfiler* profiler() { return pw_profiler; }
    QPipewireNodeListModel* nodes() { return m_nodes; }
    QList<QPipewireNode*> nodeList() { return m_nodes->list(); }
    QList<QPipewireLink*> linkList() { return m_links; }
    SystemdService* pipewireMediaSession() { return pipewire_media_session; }
    AlsaProperties* alsaProperties() { return alsa_properties; }
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
