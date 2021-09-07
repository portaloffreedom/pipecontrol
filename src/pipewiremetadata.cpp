#include "pipewiremetadata.h"

#include <spa/utils/result.h>
#include <spa/utils/string.h>
#include <spa/utils/defs.h>

#include <pipewire/filter.h>

#include <stdexcept>
#include <iostream>
#include <string>

#define LOG_LEVEL_KEY "log.level"
#define MIN_BUFFER_KEY "clock.min-quantum"
#define MAX_BUFFER_KEY "clock.max-quantum"
#define FORCE_SAMPLERATE_KEY "clock.force-rate"
#define FORCE_BUFFER_KEY "clock.force-quantum"

//-----------------------------------------------------------------------------
void PipewireMetadata::round_trip()
{
    resync();

    // The event is collected by callback _on_core_done and
    // when proper, it sets round_trip_done to true
    while(!round_trip_done) {
        pw_main_loop_run(loop);
    }
}

void PipewireMetadata::resync()
{
    round_trip_done = false;

    // This is async, will trigger a DONE event
    // with `seq` number the number just returned by this function
    sync = pw_core_sync(core, PW_ID_CORE, sync);
}

//-----------------------------------------------------------------------------
PipewireMetadata::~PipewireMetadata()
{
    if (metadata_settings != nullptr) {
        pw_proxy_destroy((struct pw_proxy*) metadata_settings);
    }
    if (client != nullptr) {
        pw_proxy_destroy((struct pw_proxy*) client);
    }
    pw_proxy_destroy((struct pw_proxy*) registry);
    pw_core_disconnect(core);
    pw_context_destroy(context);
    pw_main_loop_destroy(loop);
    pw_deinit();
}

//-----------------------------------------------------------------------------
static void do_quit(void *userdata, int signal_number)
{
    PipewireMetadata* _this = static_cast<PipewireMetadata*>(userdata);
    _this->_loop_quit();
}

void PipewireMetadata::_loop_quit()
{
    pw_main_loop_quit(loop);
}

//-----------------------------------------------------------------------------
static void on_core_done(void *data, uint32_t id, int seq)
{
    PipewireMetadata* _this = static_cast<PipewireMetadata*>(data);
    _this->_on_core_done(id, seq);
}

void PipewireMetadata::_on_core_done(uint32_t id, int seq)
{
    qDebug() << "_on_core_done("<<id<<","<<seq<<')';
    if (id == PW_ID_CORE && sync == seq)
        round_trip_done = true;
        pw_main_loop_quit(loop);
}

static void on_core_error(void *data, uint32_t id, int seq, int res, const char *message)
{
    PipewireMetadata* _this = static_cast<PipewireMetadata*>(data);
    _this->_on_core_error(id, seq, res, message);
}

void PipewireMetadata::_on_core_error(u_int32_t id, int seq, int res, const char *message)
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
static void client_info(void *data, const pw_client_info *info)
{
    PipewireMetadata* _this = static_cast<PipewireMetadata*>(data);
    _this->_client_info(info);
}

void PipewireMetadata::_client_info(const struct pw_client_info *info)
{
//    std::cout << "client: id("<<info->id<<")"<<std::endl;
//    std::cout << "\tprops" << std::endl;

//    const struct spa_dict_item *item;
//    spa_dict_for_each(item, info->props) {
//        std::cout << "\t\t"<<item->key<<": \""<<item->value<<'"'<<std::endl;
//    }
}

static const pw_client_events client_events {
    .version = PW_VERSION_CLIENT_EVENTS,
    .info = client_info,
};

//-----------------------------------------------------------------------------
static int metadata_settings_property(void *data,
                                      uint32_t id,
                                      const char* key,
                                      const char* type,
                                      const char* value)
{
    PipewireMetadata* _this = static_cast<PipewireMetadata*>(data);
    return _this->_metadata_settings_property(id, key, type, value);
}

int PipewireMetadata::_metadata_settings_property(uint32_t id,
                                     const char* key,
                                     const char* type,
                                     const char* value)
{
    std::cout << "metadata property id("<<id<<")"<<std::endl;
    if (key == nullptr) {
        std::cout << "\tremove: id("<<id<<") all keys"<<std::endl;
    } else if (value == nullptr) {
        std::cout << "\tremove: id("<<id<<") key("<<key<<')'<<std::endl;
    } else {
        std::cout << "\tupdate: id("<<id<<") key("<<key<<") value("<<value<<") type("<<type<<')'<<std::endl;

        if (strcmp(key, FORCE_SAMPLERATE_KEY)==0){
            m_force_sampleRate = std::stoi(value);
            emit force_sampleRateChanged(m_force_sampleRate);
        } else if (strcmp(key, FORCE_BUFFER_KEY)==0) {
            m_force_buffer = std::stoi(value);
            emit force_bufferChanged(m_force_buffer);
        } else if (strcmp(key, MIN_BUFFER_KEY)==0) {
            m_minBuffer = std::stoi(value);
            emit minBufferChanged(m_minBuffer);
        } else if (strcmp(key, MAX_BUFFER_KEY)==0) {
            m_maxBuffer = std::stoi(value);
            emit maxBufferChanged(m_maxBuffer);
        }
    }

    return 0;
}

static const pw_metadata_events metadata_settings_events {
    .version = PW_VERSION_METADATA_EVENTS,
    .property = metadata_settings_property,
};

//-----------------------------------------------------------------------------
static void registry_event_global(void *data,
                                  uint32_t id,
                                  uint32_t permissions,
                                  const char *type,
                                  uint32_t version,
                                  const struct spa_dict *props)
{
    PipewireMetadata* _this = static_cast<PipewireMetadata*>(data);
    _this->_registry_event(id, permissions, type, version, props);
}

void PipewireMetadata::_registry_event(uint32_t id, uint32_t permissions, const char *type, uint32_t version, const struct spa_dict *props)
{
    std::cout << "object: id("<<id<<") type("<<type<<'/'<<version<<')' << std::endl;

    if(this->client == nullptr && strcmp(type, PW_TYPE_INTERFACE_Client) == 0)
    {
        this->client = static_cast<pw_client*>(
                           pw_registry_bind(registry, id, type, PW_VERSION_CLIENT, 0));
        pw_client_add_listener(this->client,
                               &this->client_listener,
                               &client_events,
                               this);
        resync();
    }
    else if (strcmp(type, PW_TYPE_INTERFACE_Metadata) == 0)
    {
        const char *metadata_name;
        metadata_name = spa_dict_lookup(props, PW_KEY_METADATA_NAME);
        if (metadata_name != nullptr) {
            if (this->metadata_settings == nullptr &&
                    spa_streq(metadata_name, "settings"))
            {
                metadata_settings = static_cast<pw_metadata*>(
                               pw_registry_bind(registry, id, type, PW_VERSION_METADATA, 0));
                pw_metadata_add_listener(metadata_settings,
                                         &metadata_settings_listener,
                                         &metadata_settings_events,
                                         this);
                resync();
            } else {
                std::cout << "Ignoring metadata \"" << metadata_name << '"' << std::endl;
            }
        }
    }
}

static const pw_registry_events registry_events = {
    .version = PW_VERSION_REGISTRY_EVENTS,
    .global = registry_event_global,
};

//-----------------------------------------------------------------------------
void PipewireMetadata::setMinBuffer(int newMinBuffer)
{
    if (newMinBuffer == m_minBuffer) return;
    std::string value = std::to_string(newMinBuffer);
    pw_metadata_set_property(metadata_settings,
                             0,
                             MIN_BUFFER_KEY,
                             nullptr,
                             value.c_str());
    round_trip();
}


//-----------------------------------------------------------------------------
void PipewireMetadata::setMaxBuffer(int newMaxBuffer)
{
    if (newMaxBuffer == m_maxBuffer) return;
    std::string value = std::to_string(newMaxBuffer);
    pw_metadata_set_property(metadata_settings,
                             0,
                             MAX_BUFFER_KEY,
                             nullptr,
                             value.c_str());
    round_trip();
}


//-----------------------------------------------------------------------------
void PipewireMetadata::setForce_sampleRate(int newSampleRate)
{
    if (newSampleRate == m_force_sampleRate) return;
    std::string value = std::to_string(newSampleRate);
    pw_metadata_set_property(metadata_settings,
                             0,
                             FORCE_SAMPLERATE_KEY,
                             nullptr,
                             value.c_str());
    round_trip();
}

//-----------------------------------------------------------------------------
void PipewireMetadata::setForce_buffer(int newBuffer)
{
    if (newBuffer == m_force_buffer) return;
    std::string value = std::to_string(newBuffer);
    pw_metadata_set_property(metadata_settings,
                             0,
                             FORCE_BUFFER_KEY,
                             nullptr,
                             value.c_str());
    round_trip();
}

//-----------------------------------------------------------------------------
PipewireMetadata::PipewireMetadata(int *argc, char **argv[], QObject *parent)
    : QObject(parent)
{
    spa_zero(core_listener);
    spa_zero(registry_listener);
    spa_zero(metadata_settings_listener);

    pw_init(argc, argv);

    std::cout << "Compiled with libpipewire: " << pw_get_headers_version() << std::endl
              << "Linked with libpipewire:   " << pw_get_library_version() << std::endl;

    loop = pw_main_loop_new(nullptr);
    if (loop == nullptr) {
        throw std::runtime_error("Error initializing Pipewire mainloop");
    }

    pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGINT, do_quit, this);
    pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGTERM, do_quit, this);

    context = pw_context_new(pw_main_loop_get_loop(loop), nullptr, 0);
    if (context == nullptr) {
        throw std::runtime_error("Error creating pipewire context");
    }

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

    registry = pw_core_get_registry(core, PW_VERSION_REGISTRY, 0);

    pw_registry_add_listener(registry,
                             &registry_listener,
                             &registry_events,
                             this);

//    connect(this, &PipewireMetadata::force_sampleRateChanged, this, [](int s) {
//        std::cout << "###### New sample rate: " << s << std::endl;
//    });
}

















