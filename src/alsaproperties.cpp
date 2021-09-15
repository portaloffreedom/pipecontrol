#include "alsaproperties.h"
#include "src/qpipewireclient.h"
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <QFile>
#include <QFileInfo>
#include <QDir>

AlsaProperties::AlsaProperties(QPipewireClient *client, QObject *parent)
    : QObject(parent)
{
    std::string config_prefix = client->property("config.prefix").toStdString();
    const char *home_folder = std::getenv("HOME");
    if (home_folder == nullptr) {
        home_folder = "";
    }

    std::ostringstream global_conf_filename;
    global_conf_filename << "/usr/share/pipewire/"
                         << config_prefix
                         << "/alsa-monitor.conf";
    std::ostringstream user_conf_filename;
    user_conf_filename << home_folder
                       << "/.config/pipewire/"
                       << config_prefix
                       << "/alsa-monitor.conf";

    this->globalConf = global_conf_filename.str().c_str();
    this->userConf = user_conf_filename.str().c_str();

    if (std::filesystem::is_regular_file(globalConf.toStdString())) {
        this->readGlobalConf();
    }
    if (std::filesystem::is_regular_file(userConf.toStdString())) {
        this->readUserConf();
    }
}

void AlsaProperties::setBatchDisabled(bool disabled)
{
    if (_batchDisabled == disabled) return;
    _batchDisabled = disabled;
    writeUserConf();
    emit batchDisabledChanged(_batchDisabled);
}

void AlsaProperties::setPeriodSize(int newPeriod)
{
    if (_periodSize == newPeriod) return;
    _periodSize = newPeriod;
    writeUserConf();
    emit periodSizeChanged(_periodSize);
}

void AlsaProperties::readGlobalConf()
{
    readConf(globalConf);
}

void AlsaProperties::readUserConf()
{
    readConf(userConf);
}

void AlsaProperties::readConf(const QString& filename)
{
    QFile conf(filename);
    if (!conf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::ostringstream error_message;
        error_message << "Could not open configuration file " << filename.toStdString();
        std::cerr << error_message.str() << std::endl;
        throw std::runtime_error(error_message.str());
    }

    while (!conf.atEnd()) {
        QString line = conf.readLine().trimmed();

        // Skip comments
        if (line.startsWith('#')) continue;

        if (line.contains("api.alsa.disable-batch")) {
//            line.
        } else if (line.contains("api.alsa.period-size")) {

        }
    }
}

void AlsaProperties::writeUserConf() const
{
    QFileInfo confInfo(userConf);
    QDir folder = QDir(confInfo.absolutePath());
    std::cout << "Creating user conf folder: " << folder.absolutePath().toStdString() << std::endl;
    if (!folder.mkpath(".")) {
        throw std::runtime_error("Could not create user conf folder");
    }

    QFile conf(userConf);
    std::cout << "Writing user conf file: " << folder.absolutePath().toStdString() << std::endl;
    if (!conf.open(QIODevice::ReadWrite | QIODevice::Text)) {
        throw std::runtime_error("Could not write user conf file");
    }

    //TODO read file

    //TODO write with lines changed
}
