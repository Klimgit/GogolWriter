cmake_minimum_required(VERSION 3.28)
project(GogolWriter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(MinimalWriter
        main.cpp
        model/model.h
        model/model.cpp
        controller/controller.h
        controller/controller.cpp
        view/MainWindow/MainWindow.h
        view/MainWindow/MainWindow.cpp
        view/ProjectWindow/ProjectWindow.h
        view/ProjectWindow/ProjectWindow.cpp
        view/tools/MarkdownHighlighter.h
        view/tools/MarkdownHighlighter.cpp
        view/tools/MarkdownEditor.h
        view/tools/MarkdownEditor.cpp
        resources/styles/ProjectStyle.h
        resources/styles/MainStyle.h
)


target_link_libraries(MinimalWriter Qt::Core Qt::Gui Qt::Widgets)
