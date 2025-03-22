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
#include <QString>

class QPipewireClient;

class AlsaProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool batchDisabled READ batchDisabled WRITE setBatchDisabled NOTIFY batchDisabledChanged)
    Q_PROPERTY(int periodSize READ periodSize WRITE setPeriodSize NOTIFY periodSizeChanged)
Q_SIGNALS:
    void batchDisabledChanged(bool);
    void periodSizeChanged(int);

private:
    QString globalConf;
    QString userConf;

    bool _batchDisabled = false;
    int _periodSize = 1024;

public:
    explicit AlsaProperties(QPipewireClient *client, QObject *parent = nullptr);
    virtual ~AlsaProperties() = default;

    [[nodiscard]] bool batchDisabled() const { return _batchDisabled; }
    [[nodiscard]] int periodSize() const { return _periodSize; }
    void setBatchDisabled(bool disabled);
    void setPeriodSize(int newPeriod);

private:
    void readGlobalConf();
    void readUserConf();
    void readConf(const QString &filename);
    void writeUserConf() const;
};
