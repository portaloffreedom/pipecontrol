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
#include <spa/utils/dict.h>
#include <pipewire/link.h>

class QPipewire;

/**
 * @todo write docs
 */
class QPipewireLink : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(int inputPort READ inputPort NOTIFY inputPortChanged)
    Q_PROPERTY(int outputPort READ outputPort NOTIFY outputPortChanged)
    Q_PROPERTY(int inputNode READ inputNode NOTIFY inputNodeChanged)
    Q_PROPERTY(int outputNode READ outputNode NOTIFY outputNodeChanged)


Q_SIGNALS:
    void idChanged();
    void inputPortChanged();
    void outputPortChanged();
    void inputNodeChanged();
    void outputNodeChanged();

protected:
    QPipewire* pipewire = nullptr;
    const uint32_t m_id;
    const uint32_t m_input_port;
    const uint32_t m_output_port;
    const uint32_t m_input_node;
    const uint32_t m_output_node;
	pw_link* link = nullptr;

public:
    /**
     * Default constructor
     */
    QPipewireLink(QPipewire* parent, uint32_t id, const spa_dict* props);

    /**
     * Destructor
     */
    ~QPipewireLink() override;

    [[nodiscard]] int id() const { return m_id; }
    [[nodiscard]] uint32_t id_u32() const { return m_id; }
    [[nodiscard]] uint32_t inputPort() const  { return m_input_port; }
    [[nodiscard]] uint32_t outputPort() const { return m_output_port; }
    [[nodiscard]] uint32_t inputNode() const  { return m_input_node; }
    [[nodiscard]] uint32_t outputNode() const { return m_output_node; }
};

std::ostream& operator<< (std::ostream& out, const QPipewireLink& link);
