cmake_minimum_required(VERSION 3.10.2)
project(dic_project CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)

set(SOURCES
    main.cpp
    mainwindow.cpp
    selectmodewidget.cpp
    historywidget.cpp
)
qt5_add_resources (RCC_SOURCES rc.qrc)
add_executable(dic ${SOURCES} ${RCC_SOURCES})
target_link_libraries(dic Qt5::Core Qt5::Widgets Qt5::Sql)
