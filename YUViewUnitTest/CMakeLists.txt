cmake_minimum_required(VERSION 3.10)

find_package(${Qt} COMPONENTS Test REQUIRED)

macro(add_utest_project PROJECT_NAME)
  find_project_files(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR})
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})
  add_executable(${PROJECT_NAME} ${SRC_FILES})
  target_link_libraries(${PROJECT_NAME} PRIVATE YUViewLib ${Qt}::Test)
  set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "UnitTest")
  qt_deploy(${PROJECT_NAME} --no-translations)
endmacro()

add_subdirectory(filesource)
add_subdirectory(statistics)
add_subdirectory(video)
