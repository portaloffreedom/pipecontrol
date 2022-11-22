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
    explicit QPipewireProfiler(QPipewire *parent, uint32_t id, const spa_dict* props);
    virtual ~QPipewireProfiler();

    int process_info(const struct spa_pod *pod, struct QPipewireNode::driver *info);
    int process_clock(const struct spa_pod *pod, struct QPipewireNode::driver *info);
    int process_driver_block(const struct spa_pod *pod, struct point *point);
    int process_follower_block(const struct spa_pod *pod, struct point *point);

    // private
    void _profiler_profile(const struct spa_pod *pod);
};
