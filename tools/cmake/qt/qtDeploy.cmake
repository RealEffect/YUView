if(WIN32)
    find_program(deployqt windeployqt $ENV{QT_DIR}/bin)
endif()

if(NOT EXISTS ${deployqt})
    message(FATAL_ERROR "Failed to locate deployqt executable: [${deployqt}]")
endif()

function(qt_deploy PROJECT_NAME)
    list(APPEND arg ${ARGN})
    list(APPEND arg --plugindir)
    list(APPEND arg $<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins)
    list(APPEND arg $<TARGET_FILE:${PROJECT_NAME}>)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${deployqt} ${arg}
    )
endfunction()