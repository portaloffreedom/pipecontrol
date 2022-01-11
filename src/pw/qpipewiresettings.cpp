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
#include "qpipewiresettings.h"
#include <QDebug>

#define LOG_LEVEL_KEY "log.level"
#define MIN_BUFFER_KEY "clock.min-quantum"
#define MAX_BUFFER_KEY "clock.max-quantum"
#define FORCE_SAMPLERATE_KEY "clock.force-rate"
#define FORCE_BUFFER_KEY "clock.force-quantum"

//-----------------------------------------------------------------------------

QPipewireSettings::QPipewireSettings(QPipewire *parent, uint32_t id, const char *type)
    :QPipewireMetadata(parent, id, type)
{
    connect(this, &QPipewireMetadata::onKeyUpdated, this, &QPipewireSettings::keyUpdated);
}

//-----------------------------------------------------------------------------

void QPipewireSettings::keyUpdated(uint32_t id, const char *key, const char *type, const char *value)
{
    if (strcmp(key, LOG_LEVEL_KEY)==0){
        m_logLevel = static_cast<spa_log_level>(std::stoi(value));
        emit logLevelChanged(m_logLevel);
    } else if (strcmp(key, FORCE_SAMPLERATE_KEY)==0){
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
    } else {
        qWarning() << "Unrecognized option \"" << key << "\" with value \"" << value << '"';
    }
}

//-----------------------------------------------------------------------------

void QPipewireSettings::setLogLevel(int newLogLevel)
{
    if (newLogLevel == m_logLevel) return;
    std::string value = std::to_string(newLogLevel);
    this->setProperty(LOG_LEVEL_KEY, value.c_str());
}

void QPipewireSettings::setMinBuffer(int newMinBuffer)
{
    if (newMinBuffer == m_minBuffer) return;
    std::string value = std::to_string(newMinBuffer);
    this->setProperty(MIN_BUFFER_KEY, value.c_str());
}

void QPipewireSettings::setMaxBuffer(int newMaxBuffer)
{
    if (newMaxBuffer == m_maxBuffer) return;
    std::string value = std::to_string(newMaxBuffer);
    this->setProperty(MAX_BUFFER_KEY, value.c_str());
}

void QPipewireSettings::setForce_sampleRate(int newSampleRate)
{
    if (newSampleRate == m_force_sampleRate) return;
    std::string value = std::to_string(newSampleRate);
    this->setProperty(FORCE_SAMPLERATE_KEY, value.c_str());
}

void QPipewireSettings::setForce_buffer(int newBuffer)
{
    if (newBuffer == m_force_buffer) return;
    std::string value = std::to_string(newBuffer);
    this->setProperty(FORCE_BUFFER_KEY, value.c_str());
}
