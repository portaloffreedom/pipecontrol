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

#include <pipewire/pipewire.h>
#include <pipewire/extensions/metadata.h>

#include <QObject>
#include <QMap>
#include <QVariantMap>
#include <QString>
#include <QtQml>

class QPipewire;

class QPipewireClient : public QObject
{
public:

    Q_OBJECT
    Q_PROPERTY(QVariantMap properties READ properties NOTIFY propertiesChanged)
	Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(QList<QVariantMap> propertiesList READ propertiesList NOTIFY propertiesChanged)

signals:
    void propertiesChanged();
    void idChanged();
    void propertyChanged(QString key, QString value);

private:
    QPipewire *pipewire = nullptr;
	uint32_t m_id = 0;

    struct pw_client *client = nullptr;
    struct spa_hook client_listener;

    QMap<QString,QString> m_properties;

public:
    explicit QPipewireClient(QPipewire *parent, uint32_t id, const spa_dict* props);
    virtual ~QPipewireClient();

	[[nodiscard]] int id() const { return m_id; }

    QString property(const QString &key)
    {
        return m_properties[key];
    }

    bool has_property(const QString &key)
    {
        return m_properties.contains(key);
    }

    QVariantMap properties()
    {
        QVariantMap rval;
        QMap<QString, QString>::iterator i = m_properties.begin();
        while (i != m_properties.end()) {
            rval[i.key()] = QVariant::fromValue<QString>(i.value());
            ++i;
        }
        return rval;
    }

    QList<QVariantMap> propertiesList()
    {
        QList<QVariantMap> rval;
        QMap<QString, QString>::iterator i = m_properties.begin();
        while (i != m_properties.end()) {
            QVariantMap value;
            value["name"] = i.key();
            value["value"] = i.value();
            rval.append(value);
            ++i;
        }
        return rval;
    }

    void _client_info(const struct pw_client_info *info);
};
