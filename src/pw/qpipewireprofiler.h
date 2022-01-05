#pragma once

#include <spa/utils/hook.h>

#include <pipewire/proxy.h>
#include <pipewire/extensions/profiler.h>

#include <QObject>
#include "src/pw/qpipewirenode.h"

class QPipewire;

class QPipewireProfiler : public QObject
{
    Q_OBJECT
signals:

private:
    QPipewire *pipewire = nullptr;
    pw_proxy *profiler = nullptr;
    spa_hook profiler_listener;
    int check_profiler;

    struct point {
        struct QPipewireNode *driver;
        struct QPipewireNode::driver info;
    };

public:
    explicit QPipewireProfiler(QPipewire *parent, uint32_t id, const char *type);
    virtual ~QPipewireProfiler();

    int process_info(const struct spa_pod *pod, struct QPipewireNode::driver *info);
    int process_clock(const struct spa_pod *pod, struct QPipewireNode::driver *info);
    int process_driver_block(const struct spa_pod *pod, struct point *point);
    int process_follower_block(const struct spa_pod *pod, struct point *point);

    // private
    void _profiler_profile(const struct spa_pod *pod);
};
