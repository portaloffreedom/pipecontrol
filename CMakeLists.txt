cmake_minimum_required(VERSION 3.16)

project(pipecontrol VERSION 0.2.9 LANGUAGES C CXX)

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
find_package(KF5 ${KF_MIN_VERSION} COMPONENTS Kirigami2 I18n CoreAddons REQUIRED)

find_package(PkgConfig)
pkg_check_modules(libpipewire REQUIRED libpipewire-0.3)

add_definitions( -fexceptions )

set(TS_FILES resources/translations/pipecontrol_it_IT.ts)

set(PROJECT_SOURCES
        src/main.cpp
        src/pw/qpipewire.h
        src/pw/qpipewire.cpp
        src/pw/qpipewiremetadata.h
        src/pw/qpipewiremetadata.cpp
        src/pw/qpipewiresettings.h
        src/pw/qpipewiresettings.cpp
        src/pw/qpipewireclient.h
        src/pw/qpipewireclient.cpp
        src/pw/qpipewirenode.h
        src/pw/qpipewirenode.cpp
        src/pw/qpipewirealsanode.h
        src/pw/qpipewirealsanode.cpp
        src/pw/qpipewiredevice.h
        src/pw/qpipewiredevice.cpp
        src/pw/qpipewirelink.h
        src/pw/qpipewirelink.cpp
        src/pw/qpipewireport.h
        src/pw/qpipewireport.cpp
        src/pw/qpipewireprofiler.h
        src/pw/qpipewireprofiler.cpp
        src/pw/qpipewirenodelistmodel.h
        src/pw/qpipewirenodelistmodel.cpp
        src/pw/media-session/alsaproperties.h
        src/pw/media-session/alsaproperties.cpp
        src/pw/utils.h
        src/pw/utils.cpp
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
  PRIVATE -DPIPECONTROL_VERSION="${CMAKE_PROJECT_VERSION}"
  PRIVATE -DKF5_COMPILED_VERSION="${KF5_VERSION}")
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
  PRIVATE KF5::Kirigami2 KF5::I18n
)

configure_file(
    ${CMAKE_SOURCE_DIR}/resources/PipeControl.desktop
    ${CMAKE_BINARY_DIR}/PipeControl.desktop
    )

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(pipecontrol)
    qt_finalize_executable(pipecontrol)
endif()

install(TARGETS pipecontrol RUNTIME DESTINATION "bin/")
install(FILES ${CMAKE_BINARY_DIR}/PipeControl.desktop DESTINATION "share/applications/" )
install(FILES resources/pipecontrol.svg DESTINATION "share/icons/hicolor/scalable/apps/" )

#add_subdirectory("${CMAKE_SOURCE_DIR}/src/examples")
