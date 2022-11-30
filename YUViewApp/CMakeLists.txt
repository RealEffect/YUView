cmake_minimum_required(VERSION 3.10)

project(YUView)

find_project_files(APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${APP_SRC_FILES})
add_executable(${PROJECT_NAME} ${APP_SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE YUViewLib)

# copy qt runtime library to build dir.
qt_deploy(${PROJECT_NAME})

# cofigure install.
cmake_policy(SET CMP0087 NEW)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
install(CODE "execute_process(COMMAND ${deployqt} --no-translations;--plugindir;${CMAKE_INSTALL_PREFIX}/plugins;${CMAKE_INSTALL_PREFIX}/$<TARGET_FILE_NAME:${PROJECT_NAME}>)")
if(WIN32)
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION pdb OPTIONAL)
endif()
