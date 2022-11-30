cmake_minimum_required(VERSION 3.10)

# global
include(tools/cmake/qt/qtDeploy.cmake)
include(tools/cmake/findProjectFiles.cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS On)

# solution
project(YUView)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
  add_compile_options(/MP)
endif(MSVC)

# where qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QT_DIR $ENV{QT_DIR})
set(QT_VERSION_MAJOR "5")
find_program(QMAKE_EXECUTABLE NAMES qmake HINTS ${QT_DIR} PATH_SUFFIXES bin)
message(STATUS QT_DIR=${QT_DIR})
execute_process(COMMAND ${QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QT_VERSION)
message(STATUS "Qt version: ${QT_VERSION}")
if (${QT_VERSION} VERSION_GREATER "5.0.0")
  string(REPLACE "." ";" QT_VERSION_LIST ${QT_VERSION})
  list(GET QT_VERSION_LIST 0 QT_VERSION_MAJOR)
else()
  message(FATAL_ERROR "Could not find valid qt version!")
endif()
set(Qt "Qt${QT_VERSION_MAJOR}")
list(PREPEND CMAKE_PREFIX_PATH ${QT_DIR})

# sub-projects
add_subdirectory(YUViewApp)
add_subdirectory(YUViewLib)
add_subdirectory(YUViewUnitTest)

# cofigure install.
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.GPL3 DESTINATION ${CMAKE_INSTALL_PREFIX})