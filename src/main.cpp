#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QIcon>
#include <QQuickWindow>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

#include <iostream>

#include "src/qpipewire.h"
#include "src/qpipewiresettings.h"
#include "src/qpipewireclient.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);
  app.setApplicationName("PipeControl");
  app.setApplicationVersion("1.0");
  app.setQuitOnLastWindowClosed(true);
  app.setWindowIcon(QIcon(QStringLiteral("qrc:/resources/pipecontrol.png")));

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
  std::cout << "settings = " << qpipewire.settings() << std::endl;
  std::cout << "log level = " << qpipewire.settings()->logLevel() << std::endl;
  std::cout << "min buffer = " << qpipewire.settings()->minBuffer() << std::endl;
  std::cout << "max buffer = " << qpipewire.settings()->maxBuffer() << std::endl;
  std::cout << "force sample = " << qpipewire.settings()->force_sampleRate() << std::endl;
  std::cout << "force buffer = " << qpipewire.settings()->force_buffer() << std::endl;

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
