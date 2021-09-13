#include "qpipewireprofiler.h"
#include "src/qpipewire.h"
#include "src/qpipewirenode.h"

#include <spa/pod/parser.h>

#define QPIPEWIRE_CAST(x) QPipewireProfiler* _this = static_cast<QPipewireProfiler*>(x);

void profiler_profile(void *data, const struct spa_pod *pod)
{
    QPIPEWIRE_CAST(data);
    _this->_profiler_profile(pod);
}

void QPipewireProfiler::_profiler_profile(const struct spa_pod *pod)
{
    struct spa_pod *o;
    struct spa_pod_prop *p;
    point point;

    uint32_t pod_size = SPA_POD_BODY_SIZE(pod);
    for (o = static_cast<spa_pod *>(SPA_POD_BODY(pod));							\
         spa_pod_is_inside(pod, pod_size, o);				\
         o = (struct spa_pod *)spa_pod_next(o))
    {
        int res = 0;
        if (!spa_pod_is_object_type(o, SPA_TYPE_OBJECT_Profiler))
            continue;

        spa_zero(point);
        SPA_POD_OBJECT_FOREACH((struct spa_pod_object*)o, p)
        {
            switch(p->key) {
            case SPA_PROFILER_info:
                    res = process_info(&p->value, &point.info);
                    break;
            case SPA_PROFILER_clock:
                    res = process_clock(&p->value, &point.info);
                    break;
            case SPA_PROFILER_driverBlock:
                    res = process_driver_block(&p->value, &point);
                    break;
            case SPA_PROFILER_followerBlock:
                    process_follower_block(&p->value, &point);
                    break;
            default:
                    break;
            }
            if (res < 0)
                break;
        }
        if (res < 0)
            continue;
    }
}

static const struct pw_profiler_events profiler_events = {
        .version = PW_VERSION_PROFILER_EVENTS,
        .profile = profiler_profile,
};

// ----------------------------------------------------------------------------

int QPipewireProfiler::process_info(const struct spa_pod *pod, struct QPipewireNode::driver *info)
{
    return spa_pod_parse_struct(pod,
                    SPA_POD_Long(&info->count),
                    SPA_POD_Float(&info->cpu_load[0]),
                    SPA_POD_Float(&info->cpu_load[1]),
                    SPA_POD_Float(&info->cpu_load[2]),
                    SPA_POD_Int(&info->xrun_count));
}

int QPipewireProfiler::process_clock(const struct spa_pod *pod, struct QPipewireNode::driver *info)
{
    return spa_pod_parse_struct(pod,
                    SPA_POD_Int(&info->clock.flags),
                    SPA_POD_Int(&info->clock.id),
                    SPA_POD_Stringn(info->clock.name, sizeof(info->clock.name)),
                    SPA_POD_Long(&info->clock.nsec),
                    SPA_POD_Fraction(&info->clock.rate),
                    SPA_POD_Long(&info->clock.position),
                    SPA_POD_Long(&info->clock.duration),
                    SPA_POD_Long(&info->clock.delay),
                    SPA_POD_Double(&info->clock.rate_diff),
                    SPA_POD_Long(&info->clock.next_nsec));
}

int QPipewireProfiler::process_driver_block(const struct spa_pod *pod, struct point *point)
{
    char *name = NULL;
    uint32_t id = 0;
    struct QPipewireNode::measurement measure;
    struct QPipewireNode *node = nullptr;

    spa_zero(measure);
    int res = spa_pod_parse_struct(pod,
                        SPA_POD_Int(&id),
                        SPA_POD_String(&name),
                        SPA_POD_Long(&measure.prev_signal),
                        SPA_POD_Long(&measure.signal),
                        SPA_POD_Long(&measure.awake),
                        SPA_POD_Long(&measure.finish),
                        SPA_POD_Int(&measure.status),
                        SPA_POD_Fraction(&measure.latency));
    if (res < 0)
        return res;

    {
        QPipewireNode *i;
        foreach(i, pipewire->m_nodes->list()) {
            if (i->id() == id) {
                node = i;
                break;
            }
        }
    }

    if (node == nullptr)
        return -ENOENT;

    node->m_driver = node;
    node->measurement = measure;
    node->info = point->info;
    point->driver = node;

    if (measure.status != 3) {
        node->errors++;
        if (node->last_error_status == -1)
            node->last_error_status = measure.status;
    }

    emit node->driverChanged();
    emit node->activeChanged();
    emit node->waitingChanged();
    emit node->busyChanged();
    // info changed
    emit node->quantumChanged();

    return 0;
}

int QPipewireProfiler::process_follower_block(const struct spa_pod *pod, struct point *point)
{
    uint32_t id = 0;
    const char *name =  NULL;
    struct QPipewireNode::measurement measure;
    struct QPipewireNode *node = nullptr;

    spa_zero(measure);
    int res = spa_pod_parse_struct(pod,
                        SPA_POD_Int(&id),
                        SPA_POD_String(&name),
                        SPA_POD_Long(&measure.prev_signal),
                        SPA_POD_Long(&measure.signal),
                        SPA_POD_Long(&measure.awake),
                        SPA_POD_Long(&measure.finish),
                        SPA_POD_Int(&measure.status),
                        SPA_POD_Fraction(&measure.latency));
    if (res < 0)
        return res;

    {
        QPipewireNode *i;
        foreach(i, pipewire->m_nodes->list()) {
            if (i->id() == id) {
                node = i;
                break;
            }
        }
    }

    if (node == nullptr)
        return -ENOENT;

    node->measurement = measure;
    node->m_driver = point->driver;
    if (measure.status != 3) {
        node->errors++;
        if (node->last_error_status == -1)
            node->last_error_status = measure.status;
    }

    emit node->driverChanged();
    emit node->activeChanged();
    emit node->waitingChanged();
    emit node->busyChanged();

    return 0;
}

// ----------------------------------------------------------------------------
QPipewireProfiler::QPipewireProfiler(QPipewire *parent, uint32_t id, const char *type)
    : QObject(parent)
    , pipewire(parent)
{
    profiler = static_cast<pw_proxy*>(
                   pw_registry_bind(pipewire->registry, id, type, PW_VERSION_PROFILER, 0));
    if (profiler == nullptr) {
        throw std::runtime_error("Error creating profiler proxy");
    }

    pw_proxy_add_object_listener(profiler,
                                 &profiler_listener,
                                 &profiler_events,
                                 this);

    pipewire->resync();
}

QPipewireProfiler::~QPipewireProfiler()
{
    spa_hook_remove(&profiler_listener);
    pw_proxy_destroy(profiler);
}
