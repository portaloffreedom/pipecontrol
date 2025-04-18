/*
 * This file is part of the pipecontrol project.
 * Copyright (c) 2022 Matteo De Carlo.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <QObject>
#include <QList>
#include <QVariantList>
#include <QGuiApplication>
#include <QStringLiteral>

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

class QPipewireMetadata;
class QPipewireSettings;
class QPipewireLink;
class QPipewirePort;
class QPipewireDevice;
class QPipewireClient;
class AlsaProperties;

#include "src/pw/qpipewiremetadata.h"
#include "src/pw/qpipewiresettings.h"
#include "src/pw/qpipewirenode.h"
#include "src/pw/qpipewireprofiler.h"
#include "src/pw/qpipewirenodelistmodel.h"
#include "src/pw/media-session/alsaproperties.h"
#include "src/systemdservice.h"

class QPipewire : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appVersion READ appVersion NOTIFY appVersionChanged)
    Q_PROPERTY(QPipewireSettings* settings READ settings NOTIFY settingsChanged)
//    Q_PROPERTY(QPipewireClient* client READ client NOTIFY clientChanged)
    Q_PROPERTY(QPipewireNodeListModel* nodes READ nodes NOTIFY nodesChanged)
    Q_PROPERTY(QList<QPipewireNode*> nodeList READ nodeList NOTIFY nodesChanged)
    Q_PROPERTY(QList<QPipewireLink*> linkList READ linkList NOTIFY linksChanged)
    Q_PROPERTY(QList<QPipewirePort*> portList READ portList NOTIFY portsChanged)
    Q_PROPERTY(QPipewireProfiler* profiler READ profiler NOTIFY profilerChanged)
    Q_PROPERTY(SystemdService* pipewireMediaSession READ pipewireMediaSession NOTIFY pipewireMediaSessionChanged)
    Q_PROPERTY(SystemdService* wirePlumberService READ wirePlumberService NOTIFY wirePlumberServiceChanged)
    Q_PROPERTY(AlsaProperties* alsaProperties READ alsaProperties NOTIFY alsaPropertiesChanged)

Q_SIGNALS:
    void quit();
    void registryObject(uint32_t id,
                        uint32_t permissions,
                        const char *type,
                        uint32_t version,
                        const struct spa_dict *props);

    void appVersionChanged(); //bogus, never Q_EMITted
    void settingsChanged();
//    void clientChanged();
    void nodesChanged();
    void linksChanged();
    void portsChanged();
    void profilerChanged();
    void pipewireMediaSessionChanged();
    void wirePlumberServiceChanged();
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

    std::vector<QPipewireClient *> pw_clients = {};
    QPipewireSettings *pw_settings = nullptr;
    QPipewireNodeListModel *m_nodes = nullptr;
    QList<QPipewireLink*> m_links;
    QList<QPipewirePort*> m_ports;
    QList<QPipewireDevice*> m_devices;
    QPipewireProfiler *pw_profiler = nullptr;
    SystemdService *pipewire_media_session = nullptr;
    SystemdService *wireplumber_service = nullptr;
    AlsaProperties *alsa_properties = nullptr;

public:
    explicit QPipewire(int *argc, char **argv[], QObject *parent = nullptr);
    virtual ~QPipewire();

    /**
     * Executes pipewire main loop, calling callbacks and other stuff.
     * Runs in main thread and is blocking until all operations are done.
     */
    Q_INVOKABLE void round_trip();

    QString appVersion() { return QStringLiteral(PROJECT_VERSION); }
    Q_INVOKABLE QString pipewireCompiledVersion() { return QString::fromUtf8(pw_get_headers_version()); }
    Q_INVOKABLE QString pipewireLinkedVersion() { return QString::fromUtf8(pw_get_library_version()); }
    Q_INVOKABLE QString platformName() { return QGuiApplication::platformName(); }
    Q_INVOKABLE QString qtCompiledVersion() { return QStringLiteral(QT_VERSION_STR); }
    Q_INVOKABLE QString qtLinkedVersion() { return QString::fromLocal8Bit(qVersion()); }
    Q_INVOKABLE QString kframeworksCompiledVersion() { return QStringLiteral(KF6_COMPILED_VERSION); }

    Q_INVOKABLE static QString formatTime(double val);
    Q_INVOKABLE bool isPipewireMediaSession() {
        //Wireplumber may mask itself as pipewire-media-session, maybe not but let's be double sure
        return pipewire_media_session->running() && !wireplumber_service->running();
    }
    Q_INVOKABLE bool isWireplumber() {
        return wireplumber_service->running();
    }
//    QPipewireClient* client() { return pw_client; }
    QPipewireSettings* settings() { return pw_settings; }
    QPipewireProfiler* profiler() { return pw_profiler; }
    QPipewireNodeListModel* nodes() { return m_nodes; }
    QList<QPipewireNode*> nodeList() { return m_nodes->list(); }
    QList<QPipewireLink*> linkList() { return m_links; }
    QList<QPipewirePort*> portList() { return m_ports; }
    SystemdService* pipewireMediaSession() { return pipewire_media_session; }
    SystemdService* wirePlumberService() { return wireplumber_service; }
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
	friend class QPipewireLink;
	friend class QPipewirePort;
    friend class QPipewireDevice;
};
