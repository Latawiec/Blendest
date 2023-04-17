
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(EXE_LOCAL_ASSETS_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/assets/)
else ()
    set(EXE_LOCAL_ASSETS_PATH ${CMAKE_BINARY_DIR}/assets/)
endif()

function(target_add_exe_local_asset projectName inputPath outputLocalPath)

    add_custom_command(
        TARGET ${projectName}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_LIST_DIR}/${inputPath}
                ${EXE_LOCAL_ASSETS_PATH}/${outputLocalPath})

endfunction()