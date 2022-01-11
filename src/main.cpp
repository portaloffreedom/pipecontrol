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
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QIcon>
#include <QQuickWindow>
#include <QLocale>
#include <QTimer>
#include <QTranslator>
#include <KLocalizedString>
#include <KLocalizedContext>

#include "src/pw/qpipewire.h"
#include "src/pw/qpipewireclient.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

  QGuiApplication app(argc, argv);
  KLocalizedString::setApplicationDomain("pipecontrol");
  QCoreApplication::setOrganizationName("Dek");
  QCoreApplication::setOrganizationDomain("matteodroids.science");
  QCoreApplication::setApplicationName("PipeControl");

  app.setApplicationName("PipeControl");
  app.setApplicationVersion(PIPECONTROL_VERSION);
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
  QPipewire *qpipewire = new QPipewire(&argc, &argv);
  qpipewire->round_trip();

  QTimer timer;
  timer.connect(&timer, &QTimer::timeout, qpipewire, &QPipewire::round_trip);
  timer.start(100);

  qpipewire->connect(qpipewire, &QPipewire::quit, &app, &QGuiApplication::quit);

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  qmlRegisterSingletonInstance("Pipewire", 1, 0, "Pipewire", qpipewire);
  qmlRegisterAnonymousType<QPipewireClient>("Pipewire.Client", 1);
#else
  static QPipewire *s_qpipewire = qpipewire;
  qmlRegisterSingletonType<QPipewire>("Pipewire", 1, 0, "Pipewire", [](QQmlEngine*, QJSEngine*) {return static_cast<QObject*>(s_qpipewire);});
#endif

  qmlRegisterUncreatableType<QPipewireNode>("Pipewire.Node", 1, 0, "Node", "Not creatable from QML");


  QQmlApplicationEngine engine;
  engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
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
  int ret = app.exec();
  qDebug() << "End";

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  // when registering a type in `qmlRegisterSingletonType`, the object is deleted automatically by the qt engine
  // otherwise we delete it ourselves
  delete qpipewire;
#endif
  return ret;
}
