cmake_minimum_required(VERSION 3.16)

project(appS6_v2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network SerialPort)

set(PROJECT_SOURCES
        main.cpp
        cihmapps6.cpp
        cihmapps6.h
        cihmapps6.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(appS6_v2
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        csht20.h csht20.cpp
        cgpio.h  cgpio.cpp
    )
else()
    if(ANDROID)
        add_library(appS6_v2 SHARED ${PROJECT_SOURCES})
    else()
        add_executable(appS6_v2 ${PROJECT_SOURCES})
    endif()
endif()

# Qt::Network    → QTcpSocket  (communication TCP)
# Qt::SerialPort → QSerialPort (communication RS232)
target_link_libraries(appS6_v2 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::SerialPort
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appS6_v2)
endif()
set_target_properties(appS6_v2 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS appS6_v2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(appS6_v2)
endif()
