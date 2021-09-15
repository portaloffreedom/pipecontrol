#include "systemdservice.h"
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>
#include <thread>

static bool argToString(const QDBusArgument &arg, QString &out);
static bool variantToString(const QVariant &arg, QString &out);

SystemdService::SystemdService(QString serviceName, bool userService)
    : serviceName(serviceName)
    , bus(userService ? QDBusConnection::sessionBus() : QDBusConnection::systemBus())
{
    if (!serviceName.endsWith(".service")) {
        this->serviceName.append(".service");
    }

    systemd = new QDBusInterface("org.freedesktop.systemd1", "/org/freedesktop/systemd1",
                                 "org.freedesktop.systemd1.Manager", bus);

    checkIsRunning();
}

SystemdService::~SystemdService()
{
    delete systemd;
}

void SystemdService::setRunning(bool running)
{
    if (isRunning == running) return;
    if (running) {
        this->start();
    } else {
        this->stop();
    }
    isRunning = running;
    emit runningChanged(isRunning);
}

void SystemdService::_setRunning(bool running)
{
    if (isRunning == running) return;
    isRunning = running;
    emit runningChanged(isRunning);
}

bool SystemdService::checkIsRunning()
{
    bool service_found = false;
    QDBusMessage r = systemd->call("ListUnits");
    if (systemd->lastError().isValid())
        qWarning() << "Call failed: " << systemd->lastError().message();
    else {
        assert(r.signature() == "a(ssssssouso)");

        QVariantList arguments = r.arguments();
        const QDBusArgument busArgument = qvariant_cast<QDBusArgument>(arguments.at(0));
        QString busSig = busArgument.currentSignature();
        assert(busSig == r.signature());
        QDBusArgument::ElementType elementType = busArgument.currentType();
        assert(elementType == QDBusArgument::ArrayType);
        busArgument.beginArray();
        while (!busArgument.atEnd())
        {
            busArgument.beginStructure();
            // The primary unit name as string
            QString name = busArgument.asVariant().toString();

            if (name != this->serviceName) {
                // We are only interested in one service
                busArgument.endStructure();
                continue;
            }
            // Service found!

            // The human readable description string
            QString desc = busArgument.asVariant().toString();
            // The load state (i.e. whether the unit file has been loaded successfully)
            QString loaded = busArgument.asVariant().toString();
            // The active state (i.e. whether the unit is currently started or not)
            QString active = busArgument.asVariant().toString();
            // The sub state (a more fine-grained version of the active state that is specific to the unit type, which the active state is not)
            QString substate = busArgument.asVariant().toString();
            // A unit that is being followed in its state by this unit, if there is any, otherwise the empty string.
            QString following = busArgument.asVariant().toString();
            // The unit object path
            QString path = busArgument.asVariant().toString();
            // If there is a job queued for the job unit the numeric job id, 0 otherwise
            uint jobqueued = busArgument.asVariant().toUInt();
            // The job type as string
            QString jobtype = busArgument.asVariant().toString();
            // The job object path
            QString jobobject = busArgument.asVariant().toString();


//            std::cout << "Unit: " << std::endl
//                      << "\tname:      " << name.toStdString() << std::endl
//                      << "\tdec:       " << desc.toStdString() << std::endl
//                      << "\tloaded:    " << loaded.toStdString() << std::endl
//                      << "\tactive:    " << active.toStdString() << std::endl
//                      << "\tsubstate:  " << substate.toStdString() << std::endl
//                      << "\tfollowing: " << following.toStdString() << std::endl
//                      << "\tpath:      " << path.toStdString() << std::endl
//                      << "\tjobqueued: " << jobqueued << std::endl
//                      << "\tjobtype:   " << jobtype.toStdString() << std::endl
//                      << "\tjobobject: " << jobobject.toStdString() << std::endl
//                         ;

            this->_setRunning(active == "active" && substate == "running");

            busArgument.endStructure();
            service_found = true;
            break;
        }
        busArgument.endArray();
    }

    if (!service_found) {
        qWarning() << "Service \""<< serviceName <<"\"not found!";
        _setRunning(false);
    }

    return isRunning;
}

#include <chrono>

void delay(uint32_t msecs, bool processQtEvents = false)
{
    if (processQtEvents) {
        QTime dieTime= QTime::currentTime().addMSecs(msecs);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    } else {
        std::this_thread::sleep_for(std::chrono::duration<uint32_t, std::milli>(msecs));
    }
}

#define SYSTEMD_TIMEOUT 30

void SystemdService::start()
{
    checkIsRunning();
    qWarning() << "STARTING SERVICE: " << serviceName;
    if (isRunning) {
        qWarning() << "WARNING! Service " << serviceName << " was already running.";
        return;
    }
    systemd->call("StartUnit", serviceName, "fail");
    //TODO check return of above call

    for(int c=0; this->isRunning; c++) {
        if (c > SYSTEMD_TIMEOUT*2) { // 30 seconds
            qWarning() << "Failed to start service " << serviceName << "after " << SYSTEMD_TIMEOUT << "seconds.";
            return;
        }
        qWarning() << "Starting " << serviceName << " ...";
        delay(500);
        checkIsRunning();
    }
    qWarning() << "Service " << serviceName << " successfully started";
}

void SystemdService::stop()
{
    checkIsRunning();
    qWarning() << "STOPPING SERVICE: " << serviceName;
    if (!isRunning) {
        qWarning() << "WARNING! Service " << serviceName << " was already stopped.";
        return;
    }
    systemd->call("StopUnit", serviceName, "fail");
    //TODO check return of above call

    for(int c=0; !this->isRunning; c++) {
        if (c > SYSTEMD_TIMEOUT*2) { // 30 seconds
            qWarning() << "Failed to stop service " << serviceName << "after " << SYSTEMD_TIMEOUT << "seconds.";
            return;
        }
        qWarning() << "Stopping " << serviceName << " ...";
        delay(500);
        checkIsRunning();
    }
    qWarning() << "Service " << serviceName << " successfully stopped";
}

void SystemdService::restart()
{
    checkIsRunning();
    qWarning() << "RESTARTING SERVICE: " << serviceName;
    systemd->call("RestartUnit", serviceName, "fail");
    //TODO check return of above call

    _setRunning(false);
    for(int c=0; !this->isRunning; c++) {
        if (c > SYSTEMD_TIMEOUT*2) { // 30 seconds
            qWarning() << "Failed to restart service " << serviceName << "after " << SYSTEMD_TIMEOUT << "seconds.";
            return;
        }
        qWarning() << "Restarting " << serviceName << " ...";
        delay(500);
        checkIsRunning();
    }
    qWarning() << "Service " << serviceName << " successfully restarted";
}

static bool variantToString(const QVariant &arg, QString &out)
{
    int argType = arg.userType();
    if (argType == QVariant::StringList) {
        out += QLatin1Char('{');
        const QStringList list = arg.toStringList();
        for (const QString &item : list)
            out += QLatin1Char('\"') + item + QLatin1String("\", ");
        if (!list.isEmpty())
            out.chop(2);
        out += QLatin1Char('}');
    } else if (argType == QVariant::ByteArray) {
        out += QLatin1Char('{');
        QByteArray list = arg.toByteArray();
        for (int i = 0; i < list.count(); ++i) {
            out += QString::number(list.at(i));
            out += QLatin1String(", ");
        }
        if (!list.isEmpty())
            out.chop(2);
        out += QLatin1Char('}');
    } else if (argType == QVariant::List) {
        out += QLatin1Char('{');
        const QList<QVariant> list = arg.toList();
        for (const QVariant &item : list) {
            if (!variantToString(item, out))
                return false;
            out += QLatin1String(", ");
        }
        if (!list.isEmpty())
            out.chop(2);
        out += QLatin1Char('}');
    } else if (argType == QMetaType::Char || argType == QMetaType::Short || argType == QMetaType::Int
               || argType == QMetaType::Long || argType == QMetaType::LongLong) {
        out += QString::number(arg.toLongLong());
    } else if (argType == QMetaType::UChar || argType == QMetaType::UShort || argType == QMetaType::UInt
               || argType == QMetaType::ULong || argType == QMetaType::ULongLong) {
        out += QString::number(arg.toULongLong());
    } else if (argType == QMetaType::Double) {
        out += QString::number(arg.toDouble());
    } else if (argType == QMetaType::Bool) {
        out += QLatin1String(arg.toBool() ? "true" : "false");
    } else if (argType == qMetaTypeId<QDBusArgument>()) {
        argToString(qvariant_cast<QDBusArgument>(arg), out);
    } else if (argType == qMetaTypeId<QDBusObjectPath>()) {
        const QString path = qvariant_cast<QDBusObjectPath>(arg).path();
        out += QLatin1String("[ObjectPath: ");
        out += path;
        out += QLatin1Char(']');
    } else if (argType == qMetaTypeId<QDBusSignature>()) {
        out += QLatin1String("[Signature: ") + qvariant_cast<QDBusSignature>(arg).signature();
        out += QLatin1Char(']');
    } else if (argType == qMetaTypeId<QDBusUnixFileDescriptor>()) {
        out += QLatin1String("[Unix FD: ");
        out += QLatin1String(qvariant_cast<QDBusUnixFileDescriptor>(arg).isValid() ? "valid" : "not valid");
        out += QLatin1Char(']');
    } else if (argType == qMetaTypeId<QDBusVariant>()) {
        const QVariant v = qvariant_cast<QDBusVariant>(arg).variant();
        out += QLatin1String("[Variant");
        int vUserType = v.userType();
        if (vUserType != qMetaTypeId<QDBusVariant>()
                && vUserType != qMetaTypeId<QDBusSignature>()
                && vUserType != qMetaTypeId<QDBusObjectPath>()
                && vUserType != qMetaTypeId<QDBusArgument>())
            out += QLatin1Char('(') + QLatin1String(v.typeName()) + QLatin1Char(')');
        out += QLatin1String(": ");
        if (!variantToString(v, out))
            return false;
        out += QLatin1Char(']');
    } else if (arg.canConvert(QVariant::String)) {
        out += QLatin1Char('\"') + arg.toString() + QLatin1Char('\"');
    } else {
        out += QLatin1Char('[');
        out += QLatin1String(arg.typeName());
        out += QLatin1Char(']');
    }
    return true;
}

bool argToString(const QDBusArgument &busArg, QString &out)
{
    QString busSig = busArg.currentSignature();
    bool doIterate = false;
    QDBusArgument::ElementType elementType = busArg.currentType();
    if (elementType != QDBusArgument::BasicType && elementType != QDBusArgument::VariantType
            && elementType != QDBusArgument::MapEntryType)
        out += QLatin1String("[Argument: ") + busSig + QLatin1Char(' ');
    switch (elementType) {
        case QDBusArgument::BasicType:
        case QDBusArgument::VariantType:
            if (!variantToString(busArg.asVariant(), out))
                return false;
            break;
        case QDBusArgument::StructureType:
            busArg.beginStructure();
            out += QLatin1Char('%');
            doIterate = true;
            break;
        case QDBusArgument::ArrayType:
            busArg.beginArray();
            out += QLatin1Char('{');
            doIterate = true;
            break;
        case QDBusArgument::MapType:
            busArg.beginMap();
            out += QLatin1Char('{');
            doIterate = true;
            break;
        case QDBusArgument::MapEntryType:
            busArg.beginMapEntry();
            if (!variantToString(busArg.asVariant(), out))
                return false;
            out += QLatin1String(" = ");
            if (!argToString(busArg, out))
                return false;
            busArg.endMapEntry();
            break;
        case QDBusArgument::UnknownType:
        default:
            out += QLatin1String("<ERROR - Unknown Type>");
            return false;
    }
    if (doIterate && !busArg.atEnd()) {
        while (!busArg.atEnd()) {
            if (!argToString(busArg, out))
                return false;
            out += QLatin1String(", ");
        }
        out.chop(2);
    }
    switch (elementType) {
        case QDBusArgument::BasicType:
        case QDBusArgument::VariantType:
        case QDBusArgument::UnknownType:
        case QDBusArgument::MapEntryType:
            // nothing to do
            break;
        case QDBusArgument::StructureType:
            busArg.endStructure();
            out += QLatin1Char('%');
            break;
        case QDBusArgument::ArrayType:
            out += QLatin1Char('}');
            busArg.endArray();
            break;
        case QDBusArgument::MapType:
            out += QLatin1Char('}');
            busArg.endMap();
            break;
    }
    if (elementType != QDBusArgument::BasicType && elementType != QDBusArgument::VariantType
            && elementType != QDBusArgument::MapEntryType)
        out += QLatin1Char(']');
    return true;
}
