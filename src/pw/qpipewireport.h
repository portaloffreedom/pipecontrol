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

#include <qobject.h>

/**
 * @todo write docs
 */
class QPipewirePort : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint32_t m_id READ m_id WRITE setM_id NOTIFY m_idChanged)

public:
    /**
     * Default constructor
     */
    QPipewirePort();

    /**
     * Destructor
     */
    ~QPipewirePort();

    /**
     * @return the m_id
     */
    uint32_t m_id() const;

public Q_SLOTS:
    /**
     * Sets the m_id.
     *
     * @param m_id the new m_id
     */
    void setM_id(uint32_t m_id);

Q_SIGNALS:
    void m_idChanged(uint32_t m_id);

private:
    uint32_t m_m_id;
};
