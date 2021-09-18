#pragma once
#include <QObject>
#include <QString>

class QPipewireClient;

class AlsaProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool batchDisabled READ batchDisabled WRITE setBatchDisabled NOTIFY batchDisabledChanged)
    Q_PROPERTY(int periodSize READ periodSize WRITE setPeriodSize NOTIFY periodSizeChanged)
signals:
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
