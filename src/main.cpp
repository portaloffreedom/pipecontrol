#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QIcon>
#include <QQuickWindow>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

#include <iostream>

#include "src/pipewiremetadata.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);
  app.setWindowIcon(QIcon("resources/pipecontrol.png"));

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
  PipewireMetadata pw_metadata(&argc, &argv);
  pw_metadata.round_trip();
  std::cout << "min buffer = " << pw_metadata.minBuffer() << std::endl;
  std::cout << "max buffer = " << pw_metadata.maxBuffer() << std::endl;
  std::cout << "force sample = " << pw_metadata.force_sampleRate() << std::endl;
  std::cout << "force buffer = " << pw_metadata.force_buffer() << std::endl;

  QTimer timer;
  timer.connect(&timer, &QTimer::timeout, &pw_metadata, &PipewireMetadata::round_trip);
  timer.start(100);

  qmlRegisterSingletonInstance("PipewireMetadata", 1, 0, "PipewireMetadata", &pw_metadata);


  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/resources/main.qml"));
  QObject::connect(&engine,
                   &QQmlApplicationEngine::objectCreated,
                   &app,
                   [url](QObject *obj, const QUrl &objUrl) {
                      if (!obj && url == objUrl)
                        QCoreApplication::exit(-1);
                   },
                   Qt::QueuedConnection);
  engine.load(url);

  if (engine.rootObjects().isEmpty()) {
      qCritical("Engine did not load any root object");
      return -1;
  }

  qDebug() << "Start";
  return app.exec();
  qDebug() << "End";
}
