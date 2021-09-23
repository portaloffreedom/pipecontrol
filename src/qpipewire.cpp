#include "qpipewire.h"
#include "src/qpipewiresettings.h"
#include "src/qpipewireclient.h"

#include <spa/utils/result.h>
#include <spa/utils/string.h>
#include <spa/utils/defs.h>
#include <spa/pod/parser.h>
#include <spa/debug/pod.h>

#include <pipewire/impl.h>
#include <pipewire/extensions/profiler.h>

#include <QDebug>

#include <stdexcept>

#define QPIPEWIRE_CAST(x) QPipewire* _this = static_cast<QPipewire*>(x);

void QPipewire::round_trip()
{
    resync();

    // The event is collected by callback _on_core_done and
    // when proper, it sets round_trip_done to true
    while(!round_trip_done) {
        pw_main_loop_run(loop);
    }
}

QString QPipewire::formatTime(double val)
{
    val *= 1000000000.0f;
    QString buf;
    if (val < 1000000llu) {
        val /= 1000.0f;
        buf = QString::asprintf("%5.1fµs", val);
    } else if (val < 1000000000llu) {
        val /= 1000000.0f;
        buf = QString::asprintf("%5.1fms", val);
    } else {
        val /= 1000000000.0f;
        buf = QString::asprintf("%5.1fs", val);
    }
    return buf;
}

void QPipewire::resync()
{
    round_trip_done = false;

    // This is async, will trigger a DONE event
    // with `seq` number the number just returned by this function
    sync = pw_core_sync(core, PW_ID_CORE, sync);
}

//-----------------------------------------------------------------------------
static void do_quit(void *userdata, int /*signal_number*/)
{
    QPIPEWIRE_CAST(userdata);
    _this->_quit();
}

void QPipewire::_quit()
{
    pw_main_loop_quit(loop);
    emit quit();
}

//-----------------------------------------------------------------------------
static void on_core_done(void *data, uint32_t id, int seq)
{
    QPIPEWIRE_CAST(data);
    _this->_on_core_done(id, seq);
}

void QPipewire::_on_core_done(uint32_t id, int seq)
{
    //qDebug() << "_on_core_done(" << id << ',' << seq << ')';
    if (id == PW_ID_CORE && sync == seq) {
        round_trip_done = true;
        pw_main_loop_quit(loop);
    }
}

static void on_core_error(void *data, uint32_t id, int seq, int res, const char *message)
{
    QPIPEWIRE_CAST(data);
    _this->_on_core_error(id, seq, res, message);
}

void QPipewire::_on_core_error(u_int32_t id, int seq, int res, const char *message)
{
    pw_log_error("error id:%u seq:%d res:%d (%s): %s",
                    id, seq, res, spa_strerror(res), message);

    if (id == PW_ID_CORE && res == -EPIPE)
         pw_main_loop_quit(loop);
}

static const pw_core_events core_events = {
    .version = PW_VERSION_CORE_EVENTS,
    .done = on_core_done,
    .error = on_core_error,
};

//-----------------------------------------------------------------------------
static void registry_event(void *data,
                                  uint32_t id,
                                  uint32_t permissions,
                                  const char *type,
                                  uint32_t version,
                                  const struct spa_dict *props)
{
    QPIPEWIRE_CAST(data);
    _this->_registry_event(id, permissions, type, version, props);
}

static void registry_event_remove(void *data, uint32_t id)
{
    QPIPEWIRE_CAST(data);
    _this->_registry_event_remove(id);
}

void QPipewire::_registry_event(uint32_t id, uint32_t permissions, const char *type, uint32_t version, const struct spa_dict *props)
{
    qDebug() << "object: id(" << id << ") type(" << type << '/' << version << ')';

    emit registryObject(id, permissions, type, version, props);

    if(pw_client == nullptr && strcmp(type, PW_TYPE_INTERFACE_Client) == 0)
    {
        pw_client = new QPipewireClient(this, id, type);
        emit clientChanged();
    }
    else if (strcmp(type, PW_TYPE_INTERFACE_Metadata) == 0)
    {
        const char *metadata_name;
        metadata_name = spa_dict_lookup(props, PW_KEY_METADATA_NAME);
        if (metadata_name != nullptr)
        {
            if (this->pw_settings == nullptr &&
                    spa_streq(metadata_name, "settings"))
            {
                pw_settings = new QPipewireSettings(this, id, type);
                emit settingsChanged();
            } else {
                qWarning() << "Ignoring metadata \"" << metadata_name << '"';
            }
        }
    }
    else if (spa_streq(type, PW_TYPE_INTERFACE_Profiler))
    {
        if (pw_profiler != nullptr) {
            qWarning() << "Ignoring profiler " << id << ": already attached";
            return;
        }
        pw_profiler = new QPipewireProfiler(this, id, type);
        emit profilerChanged();
    }
    else if (spa_streq(type, PW_TYPE_INTERFACE_Node))
    {
        QPipewireNode *node = new QPipewireNode(this, id, props);
        m_nodes->append(node);
        qWarning() << "Adding node id(" << id << "): " << node->name();
        emit nodesChanged();
    }
}

void QPipewire::_registry_event_remove(uint32_t id)
{
    qWarning() << "Attempting to remove id(" << id << ")";
    QPipewireNode *node = nullptr;
    for(int i=0; i<m_nodes->size(); i++) {
        QPipewireNode *candidate = (*m_nodes)[i];
        if (candidate && candidate->id() == id) {
            node = candidate;
            break;
        }
    }
    if (node != nullptr) {
        // ALL interfaces that are not NODES are ignored here.
        qWarning() << "Removing id(" << id << ":" << node->id() << "): " << node->name();
        int index = m_nodes->list().indexOf(node);
        m_nodes->removeOne(index);
        emit nodesChanged();
        node->deleteLater();
    }
}

static const pw_registry_events registry_events = {
    .version = PW_VERSION_REGISTRY_EVENTS,
    .global = registry_event,
    .global_remove = registry_event_remove,
};

//-----------------------------------------------------------------------------
QPipewire::QPipewire(int *argc, char **argv[], QObject *parent)
    : QObject(parent)
    , m_nodes(new QPipewireNodeListModel())
{
    spa_zero(core_listener);
    spa_zero(registry_listener);

    pw_init(argc, argv);

    qInfo() << "Compiled with libpipewire " << pw_get_headers_version();
    qInfo() << "Linked   with libpipewire " << pw_get_library_version();

    loop = pw_main_loop_new(nullptr);
    if (loop == nullptr) {
        throw std::runtime_error("Error initializing Pipewire mainloop");
    }

    pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGINT, do_quit, this);
    pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGTERM, do_quit, this);

    context = pw_context_new(pw_main_loop_get_loop(loop), nullptr, 0);
    if (context == nullptr) {
        throw std::runtime_error("Error creating Pipewire context");
    }

    // Need this line or the profiler will not load.
    pw_context_load_module(context, PW_EXTENSION_MODULE_PROFILER, nullptr, nullptr);

    //TODO remote should be?
    const char *remote = "";
    core = pw_context_connect(context,
                              pw_properties_new(PW_KEY_REMOTE_NAME, remote, nullptr),
                              0);
    if (core == nullptr) {
        throw std::runtime_error("Can't connect to pipewire");
    }

    pw_core_add_listener(core,
                         &core_listener,
                         &core_events,
                         this);

    registry = pw_core_get_registry(core,
                                    PW_VERSION_REGISTRY,
                                    0);
    pw_registry_add_listener(registry,
                             &registry_listener,
                             &registry_events,
                             this);

    pipewire_media_session = new SystemdService("pipewire-media-session", true);

    // QT stuff
    connect(m_nodes, &QPipewireNodeListModel::layoutChanged, this, [this]() {
        emit nodesChanged();
    });
}

QPipewire::~QPipewire()
{
    delete pipewire_media_session;

    if (pw_settings != nullptr) {
        delete pw_settings;
    }
    if (pw_client != nullptr) {
        delete pw_client;
    }
    if (pw_profiler != nullptr) {
        delete pw_profiler;
    }
    QPipewireNode *node;
    foreach(node, m_nodes->list()) {
        delete node;
    }
    delete m_nodes;
    pw_proxy_destroy((struct pw_proxy*) registry);
    pw_core_disconnect(core);
    pw_context_destroy(context);
    pw_main_loop_destroy(loop);
    pw_deinit();
}
