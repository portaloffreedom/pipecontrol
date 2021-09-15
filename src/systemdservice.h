#pragma once
#include <QString>
#include <QtDBus/QtDBus>

class SystemdService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
signals:
    void runningChanged(bool);

private:
    QString serviceName;
    QDBusConnection bus;
    QDBusInterface *systemd = nullptr;
    bool isRunning = false;

public:
    SystemdService(QString serviceName, bool userService = true);
    ~SystemdService();

    bool running() { return isRunning; }
    void setRunning(bool);

    bool checkIsRunning();
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void restart();

private:
    // Sets variable, emits signal but does not communicate with systemd
    void _setRunning(bool);
};
