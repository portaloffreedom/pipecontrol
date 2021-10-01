cmake_minimum_required(VERSION 3.16)

project(pipecontrol VERSION 0.1.4 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(KF_MIN_VERSION "5.68.0")
set(QT_MIN_VERSION "5.12.0")
set(QT_VERSION_MAJOR "5")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

#find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick LinguistTools QuickControls2 DBus REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick LinguistTools QuickControls2 DBus REQUIRED)
find_package(Qt5 ${QT_MIN_VERSION} COMPONENTS Core Quick LinguistTools QuickControls2 DBus REQUIRED)
#find_package(KF5 ${KF_MIN_VERSION} COMPONENTS Kirigami2 CoreAddons REQUIRED)

find_package(PkgConfig)
pkg_check_modules(libpipewire REQUIRED libpipewire-0.3)

add_definitions( -fexceptions )

set(TS_FILES resources/translations/pipecontrol_it_IT.ts)

set(PROJECT_SOURCES
        src/main.cpp
        src/qpipewire.h
        src/qpipewire.cpp
        src/qpipewiremetadata.h
        src/qpipewiremetadata.cpp
        src/qpipewiresettings.h
        src/qpipewiresettings.cpp
        src/qpipewireclient.h
        src/qpipewireclient.cpp
        src/qpipewirenode.h
        src/qpipewirenode.cpp
        src/qpipewireprofiler.h
        src/qpipewireprofiler.cpp
        src/qpipewirenodelistmodel.h
        src/qpipewirenodelistmodel.cpp
        src/alsaproperties.h
        src/alsaproperties.cpp
        src/systemdservice.h
        src/systemdservice.cpp
        qml.qrc
        ${TS_FILES}
)

if("${QT_VERSION_MAJOR}" GREATER_EQUAL 6)
    qt_add_executable(pipecontrol
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(pipecontrol
      ${PROJECT_SOURCES}
    )
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_compile_definitions(pipecontrol
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
  PRIVATE -DPIPECONTROL_VERSION="${CMAKE_PROJECT_VERSION}")
target_compile_options(pipecontrol
  PRIVATE ${libpipewire_CFLAGS_OTHER})
target_compile_definitions(pipecontrol PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}")
target_include_directories(pipecontrol
  PRIVATE ${libpipewire_INCLUDE_DIRS})
target_link_libraries(pipecontrol
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickControls2 Qt${QT_VERSION_MAJOR}::DBus
  PRIVATE ${libpipewire_LIBRARIES}
#  PRIVATE KF5::Kirigami2
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(pipecontrol)
    qt_finalize_executable(pipecontrol)
endif()

install(TARGETS pipecontrol RUNTIME DESTINATION "bin/")
install(FILES resources/PipeControl.desktop DESTINATION "share/applications/" )
install(FILES resources/pipecontrol.png DESTINATION "share/icons/" )
