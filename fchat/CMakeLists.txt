 cmake_minimum_required(VERSION 3.19)
project(fchat LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network)
qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

set(CONFIG_IN ${CMAKE_CURRENT_SOURCE_DIR}/config.ini)

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


    singleton.h
    httpmgr.h httpmgr.cpp
    config.ini
    global.h
    timer_button.h timer_button.cpp
    click_label.h click_label.cpp
    reset_dialog.h reset_dialog.cpp resetdialog.ui
)

include_directories(${PROJECT_SOURCE_DIR})

target_link_libraries(fchat
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

# ---------- 把 config.ini 复制到输出目录 ----------
add_custom_command(TARGET fchat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CONFIG_IN}"
            "$<TARGET_FILE_DIR:fchat>/config.ini"
    COMMENT "Copy config.ini next to the executable")
#       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# $<TARGET_FILE_DIR:...> 这一行会在 Debug/Release 里自动变成正确的

include(GNUInstallDirs)

install(TARGETS fchat
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 把 config.ini 安装到同一目录
install(FILES "${CONFIG_IN}" DESTINATION ${CMAKE_INSTALL_BINDIR})

qt_generate_deploy_app_script(
    TARGET fchat
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
