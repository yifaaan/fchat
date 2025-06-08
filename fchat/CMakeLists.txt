cmake_minimum_required(VERSION 3.19)
project(fchat LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network)
qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(fchat
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources/icons/app.png
    resources.qrc
    logindialog.h logindialog.cpp logindialog.ui
    resources/icons/login.jpg
    registerdialog.h registerdialog.cpp registerdialog.ui
    global.h
    global.cpp
    singleton.h
    httpmgr.h httpmgr.cpp
)

target_link_libraries(fchat
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

include(GNUInstallDirs)

install(TARGETS fchat
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET fchat
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
