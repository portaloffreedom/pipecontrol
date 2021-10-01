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
    Q_PROPERTY(QList<QVariantMap> propertiesList READ propertiesList NOTIFY propertiesChanged)

signals:
    void propertiesChanged();
    void propertyChanged(QString key, QString value);

private:
    QPipewire *pipewire = nullptr;

    struct pw_client *client = nullptr;
    struct spa_hook client_listener;

    QMap<QString,QString> m_properties;

public:
    explicit QPipewireClient(QPipewire *parent, uint32_t id, const char *type);
    virtual ~QPipewireClient();

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
