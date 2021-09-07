#pragma once

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

#include <QObject>
#include <qqml.h>

class PipewireMetadata : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int minBuffer READ minBuffer WRITE setMinBuffer NOTIFY minBufferChanged)
    Q_PROPERTY(int maxBuffer READ maxBuffer WRITE setMaxBuffer NOTIFY maxBufferChanged)
    Q_PROPERTY(int force_sampleRate READ force_sampleRate WRITE setForce_sampleRate NOTIFY force_sampleRateChanged)
    Q_PROPERTY(int force_buffer READ force_buffer WRITE setForce_buffer NOTIFY force_bufferChanged)

signals:
    void force_sampleRateChanged(int);
    void force_bufferChanged(int);
    void minBufferChanged(int);
    void maxBufferChanged(int);

private:
    struct pw_main_loop *loop = nullptr;

    struct pw_context *context = nullptr;

    struct pw_core *core = nullptr;
    struct spa_hook core_listener;

    struct pw_client *client = nullptr;
    struct spa_hook client_listener;

    struct pw_registry *registry = nullptr;
    struct spa_hook registry_listener;

    struct pw_metadata *metadata_settings = nullptr;
    struct spa_hook metadata_settings_listener;

    int sync = 0;
    bool round_trip_done = false;

    int m_minBuffer = 0;
    int m_maxBuffer = 0;
    int m_force_sampleRate = 0;
    int m_force_buffer = 0;

public:
    PipewireMetadata(int *argc, char **argv[], QObject *parent = 0);
    virtual ~PipewireMetadata();

    /**
     * Executes pipewire main loop, calling callbacks and other stuff.
     * Runs in main thread and is blocking until all operations are done.
     */
    Q_INVOKABLE void round_trip();

    int minBuffer() { return m_minBuffer; }
    int maxBuffer() { return m_maxBuffer; }
    int force_sampleRate() { return m_force_sampleRate; }
    int force_buffer() { return m_force_buffer; }

    void setMinBuffer(int newMinBuffer);
    void setMaxBuffer(int newMaxBuffer);
    void setForce_sampleRate(int newSampleRate);
    void setForce_buffer(int newBuffer);

    // PIPEWIRES CALLBACKS
    void _loop_quit();
    void _on_core_done(uint32_t id, int seq);
    void _on_core_error(u_int32_t id, int seq, int res, const char *message);
    void _client_info(const struct pw_client_info *info);
    void _registry_event(uint32_t id,
                         uint32_t permissions,
                         const char *type,
                         uint32_t version,
                         const struct spa_dict *props);
    int _metadata_settings_property(uint32_t id,
                                    const char* key,
                                    const char* type,
                                    const char* value);

private:
    void resync();
};
