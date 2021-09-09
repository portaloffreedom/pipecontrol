cmake_minimum_required(VERSION 3.14)

project(pipecontrol VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick LinguistTools QuickControls2 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick LinguistTools QuickControls2 REQUIRED)

find_package(PkgConfig)
pkg_check_modules(libpipewire REQUIRED libpipewire-0.3)

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
        qml.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
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
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_compile_options(pipecontrol
  PRIVATE ${libpipewire_CFLAGS_OTHER})
target_include_directories(pipecontrol
  PRIVATE ${libpipewire_INCLUDE_DIRS})
target_link_libraries(pipecontrol
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickControls2
  PRIVATE ${libpipewire_LIBRARIES})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(pipecontrol)
    qt_finalize_executable(pipecontrol)
endif()
