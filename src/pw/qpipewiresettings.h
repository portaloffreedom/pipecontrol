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

class QPipewireMetadata;
class QPipewire;

#include "src/pw/qpipewiremetadata.h"

class QPipewireSettings : public QPipewireMetadata
{
    Q_OBJECT
    Q_PROPERTY(int logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
    Q_PROPERTY(int minBuffer READ minBuffer WRITE setMinBuffer NOTIFY minBufferChanged)
    Q_PROPERTY(int maxBuffer READ maxBuffer WRITE setMaxBuffer NOTIFY maxBufferChanged)
    Q_PROPERTY(int force_sampleRate READ force_sampleRate WRITE setForce_sampleRate NOTIFY force_sampleRateChanged)
    Q_PROPERTY(int force_buffer READ force_buffer WRITE setForce_buffer NOTIFY force_bufferChanged)

signals:
    void logLevelChanged(int);
    void force_sampleRateChanged(int);
    void force_bufferChanged(int);
    void minBufferChanged(int);
    void maxBufferChanged(int);

private:
    int m_logLevel = static_cast<int>(spa_log_level::SPA_LOG_LEVEL_WARN);
    int m_minBuffer = 0;
    int m_maxBuffer = 0;
    int m_force_sampleRate = 0;
    int m_force_buffer = 0;

public:
    explicit QPipewireSettings(QPipewire *parent, uint32_t id, const spa_dict* props);
    virtual ~QPipewireSettings() = default;

    int logLevel() { return m_logLevel; }
    int minBuffer() { return m_minBuffer; }
    int maxBuffer() { return m_maxBuffer; }
    int force_sampleRate() { return m_force_sampleRate; }
    int force_buffer() { return m_force_buffer; }

    void setLogLevel(int newLogLevel);
    void setMinBuffer(int newMinBuffer);
    void setMaxBuffer(int newMaxBuffer);
    void setForce_sampleRate(int newSampleRate);
    void setForce_buffer(int newBuffer);

private:
    void keyUpdated(uint32_t id, const char* key, const char* type, const char* value);
};
