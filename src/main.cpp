#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QIcon>
#include <QQuickWindow>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

#include "src/qpipewire.h"
#include "src/qpipewiresettings.h"
#include "src/qpipewireclient.h"
#include "src/systemdservice.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);
  app.setApplicationName("PipeControl");
  app.setApplicationVersion("1.0");
  app.setQuitOnLastWindowClosed(true);
  app.setWindowIcon(QIcon(QStringLiteral(INSTALL_PREFIX"/share/icons/pipecontrol.png")));

  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for (const QString &locale : uiLanguages) {
      const QString baseName = "pipecontrol_" + QLocale(locale).name();
      if (translator.load(":/i18n/" + baseName)) {
          app.installTranslator(&translator);
          break;
        }
    }

  // Allocate before the engine to ensure that it outlives it
  QPipewire qpipewire(&argc, &argv);
  qpipewire.round_trip();

  QTimer timer;
  timer.connect(&timer, &QTimer::timeout, &qpipewire, &QPipewire::round_trip);
  timer.start(100);

  qpipewire.connect(&qpipewire, &QPipewire::quit, &app, &QGuiApplication::quit);

  qmlRegisterSingletonInstance("Pipewire", 1, 0, "Pipewire", &qpipewire);
  qmlRegisterAnonymousType<QPipewireClient>("Pipewire.Client", 1);
  qmlRegisterAnonymousType<QPipewireNode>("Pipewire.Node", 1);

  QQmlApplicationEngine engine;
  const QUrl urlMain(QStringLiteral("qrc:/resources/main.qml"));
  QObject::connect(&engine,
                   &QQmlApplicationEngine::objectCreated,
                   &app,
                   [urlMain](QObject *obj, const QUrl &objUrl) {
                      if (!obj && urlMain == objUrl)
                        QCoreApplication::exit(-1);
                   },
                   Qt::QueuedConnection);
  engine.load(urlMain);

  if (engine.rootObjects().isEmpty()) {
      qCritical("Engine did not load any root object");
      return -1;
  }

  qDebug() << "Start";
  return app.exec();
  qDebug() << "End";
}
