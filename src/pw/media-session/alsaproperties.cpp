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
#include "alsaproperties.h"
#include "src/pw/qpipewireclient.h"
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>

bool is_valid_file(const QString &path)
{
    QFile file(path);
    if (!file.exists()) return false;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    return file.isReadable();
}

AlsaProperties::AlsaProperties(QPipewireClient *client, QObject *parent)
    : QObject(parent)
{
    std::string config_prefix = client->has_property(QStringLiteral("config.prefix")) ?
                                    client->property(QStringLiteral("config.prefix")).toStdString() : "media-session.d";
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

    this->globalConf = QString::fromUtf8(global_conf_filename.str().c_str());
    this->userConf = QString::fromUtf8(user_conf_filename.str().c_str());

    bool has_global = is_valid_file(globalConf);
    bool has_user = is_valid_file(userConf);

    if (has_user) {
        this->readUserConf();
    } else if (has_global) {
        this->readGlobalConf();
    } else {
        throw std::runtime_error("Could not find pipewire alsa monitor config file!");
    }
}

void AlsaProperties::setBatchDisabled(bool disabled)
{
    if (_batchDisabled == disabled) return;
    _batchDisabled = disabled;
    writeUserConf();
    Q_EMIT batchDisabledChanged(_batchDisabled);
}

void AlsaProperties::setPeriodSize(int newPeriod)
{
    if (_periodSize == newPeriod) return;
    _periodSize = newPeriod;
    writeUserConf();
    Q_EMIT periodSizeChanged(_periodSize);
}

void AlsaProperties::readGlobalConf()
{
    readConf(globalConf);
}

void AlsaProperties::readUserConf()
{
    readConf(userConf);
}

template<typename T>
T _parse(const QString &/*value*/) {
    throw std::runtime_error("Unsupported conversion");
}
template<>
int _parse<int>(const QString &value) {
    return value.toInt();
}
template<>
bool _parse<bool>(const QString &value) {
    const QString lower_case_value = value.toLower();
    if (lower_case_value == QStringLiteral("true")) {
        return true;
    } else if (lower_case_value == QStringLiteral("false")) {
        return false;
    } else {
        throw std::runtime_error(QString((QStringLiteral("Could not parse bool type from: ") + value)).toStdString());
    }
}

template<typename T>
QString _tostring(const T /*value*/) {
    throw std::runtime_error("diocane");
}
template<>
QString _tostring<int>(const int value) {
    return QString::number(value);
}
template<>
QString _tostring<bool>(const bool value) {
    if (value) {
        return QStringLiteral("true");
    } else {
        return QStringLiteral("false");
    }
}

template<typename T>
bool parse_line(const QString &line, const QString &target, T &value)
{
    QRegularExpression re(QStringLiteral("(#*)\\s*") + target + QStringLiteral("\\s*=\\s*([\\d|\\w]+)"));
    QRegularExpressionMatch match = re.match(line);
    if(!match.hasMatch()) {
        throw std::runtime_error(QString((QStringLiteral("Could not find ") + target)).toStdString());
    }

    if (!match.captured(1).isEmpty()) {
        return false;
    }

    value = _parse<T>(match.captured(2));
    return true;
}

template<typename T>
void parse_and_change_line(QString &line, const QString &target, T value)
{
    QRegularExpression re(QStringLiteral("(#*)\\s*") + target + QStringLiteral("\\s*=\\s*([\\d|\\w]+)"));
    QRegularExpressionMatch match = re.match(line);
    if(!match.hasMatch()) {
        throw std::runtime_error(QString((QStringLiteral("Could not find ") + target)).toStdString());
    }

    if (!match.captured(1).isEmpty()) {
        line.remove(match.capturedStart(1), match.capturedLength(1));
        match = re.match(line);
    }

    line.remove(match.capturedStart(2), match.capturedLength(2));
    line.insert(match.capturedStart(2), _tostring<T>(value));
    return;
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
        QString line = QString::fromUtf8(conf.readLine().trimmed());

        // Skip comments
        if (line.startsWith(QStringLiteral("#"))) continue;

        if (line.contains(QStringLiteral("api.alsa.disable-batch"))) {
            bool found = parse_line(line, QStringLiteral("api.alsa.disable-batch"), _batchDisabled);
            if (found) Q_EMIT batchDisabledChanged(_batchDisabled);
        } else if (line.contains(QStringLiteral("api.alsa.period-size"))) {
            bool found = parse_line(line, QStringLiteral("api.alsa.period-size"), _periodSize);
            if (found) Q_EMIT periodSizeChanged(_periodSize);
        }
    }
}

void AlsaProperties::writeUserConf() const
{
    QFileInfo confInfo(userConf);
    QDir folder = QDir(confInfo.absolutePath());
    std::cout << "Creating user conf folder: " << folder.absolutePath().toStdString() << std::endl;
    // This will return true even if the folder already exists.
    if (!folder.mkpath(QStringLiteral("."))) {
        throw std::runtime_error("Could not create user conf folder");
    }

    // Read file
    QList<QString> lines;
    {
        QFile userConfF(userConf);
        QFile globalConfF(globalConf);
        QFile *inputFile = &userConfF;
        if (!userConfF.exists()) {
            inputFile = &globalConfF;
        }

        if (!inputFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("Could not write user conf file");
        }

        while (!inputFile->atEnd()) {
            QString line = QString::fromUtf8(inputFile->readLine());
            QString trimmed = line.trimmed();
            if (trimmed.contains(QStringLiteral("api.alsa.disable-batch"))) {
                parse_and_change_line(line, QStringLiteral("api.alsa.disable-batch"), this->_batchDisabled);
            } else if (trimmed.contains(QStringLiteral("api.alsa.period-size"))) {
                parse_and_change_line(line, QStringLiteral("api.alsa.period-size"), this->_periodSize);
            }

            lines.append(line);
        }

        inputFile->close();
    }

    std::cout << "Writing user conf file: " << folder.absolutePath().toStdString() << std::endl;

    {
        // Write with lines changed
        QFile outFile(userConf);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            throw std::runtime_error("Could not write user conf file");
        }

        QTextStream out(&outFile);
        for(QString &line : lines) {
            out << line;
        }

        outFile.close();
    }

}
