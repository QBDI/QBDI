

macro(MERGE_STATIC_LIBS TARGET OUTPUT_NAME LIBS_TO_MERGE)

    set(SOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_depends.c)

    add_custom_command(
        OUTPUT  ${SOURCE_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake/dummy.c ${SOURCE_FILE}
        DEPENDS ${LIBS_TO_MERGE})

    add_library(${TARGET} STATIC ${SOURCE_FILE})
    set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})

    set(LIBS_PATH "")
    foreach (LIB ${LIBS_TO_MERGE})
        if (TARGET ${LIB})
            add_dependencies(${TARGET} ${LIB})
            set(LIBS_PATH ${LIBS_PATH} $<TARGET_FILE:${LIB}>)
        else ()
            set(LIBS_PATH ${LIBS_PATH} ${LIB})
        endif()
    endforeach()
    list(REMOVE_DUPLICATES LIBS_PATH)

    if (WIN32)

        set(LIB_COMMAND_ARGS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_lib_exe_args")
        string(JOIN " " LINK_LIBS ${LIBS_PATH})
        file(GENERATE OUTPUT ${LIB_COMMAND_ARGS_FILE} CONTENT
            "/OUT:$<TARGET_FILE:${TARGET}> ${LINK_LIBS}")

        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:${TARGET}>
            COMMAND "LIB.EXE" "@${LIB_COMMAND_ARGS_FILE}")

    elseif(APPLE)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:${TARGET}>
            COMMAND /usr/bin/libtool -static -o $<TARGET_FILE:${TARGET}> ${LIBS_PATH}
        )

    elseif(UNIX)
        set(AR_COMMAND_ARGS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_ar_script_file")

        set(AR_SCRIPT "")
        foreach (LIB ${LIBS_PATH})
            set(AR_SCRIPT "${AR_SCRIPT}\naddlib ${LIB}")
        endforeach()

        file(GENERATE OUTPUT ${AR_COMMAND_ARGS_FILE} CONTENT
            "create $<TARGET_FILE:${TARGET}>\n${AR_SCRIPT}\nsave\nend\n")

        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:${TARGET}>
            COMMAND ${CMAKE_AR} "-M" "<${AR_COMMAND_ARGS_FILE}")

    else()
        message(FATAL_ERROR "Static llvm library: unsupported system ${CMAKE_SYSTEM_NAME}")
    endif()



endmacro()

