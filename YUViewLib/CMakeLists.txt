cmake_minimum_required(VERSION 3.10)

project(YUViewLib)

find_package(${Qt} COMPONENTS Core Gui Widgets Opengl Xml Concurrent Network REQUIRED)

find_project_files(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR})
find_ui_files(UI_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ui)
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES} ${UI_FILES})
add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${UI_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PUBLIC
    ${Qt}::Gui
    ${Qt}::Widgets
    ${Qt}::OpenGL
    ${Qt}::Xml
    ${Qt}::Concurrent
    ${Qt}::Network
)
