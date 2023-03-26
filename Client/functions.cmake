

set(EXE_LOCAL_ASSETS_PATH ${CMAKE_BINARY_DIR}/assets/)

function(target_add_exe_local_asset projectName inputPath outputLocalPath)

    add_custom_command(
        TARGET ${projectName}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_LIST_DIR}/${inputPath}
                ${EXE_LOCAL_ASSETS_PATH}/${outputLocalPath})

endfunction()