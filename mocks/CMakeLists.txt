cmake_minimum_required(VERSION 3.28)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(SlidingWindowLZ77_Mocks LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    find_package(Qt6 6.12 REQUIRED COMPONENTS Core Gui Widgets)
endif()

if(WIN32 AND NOT MSVC AND NOT MINGW)
    message(FATAL_ERROR "SlidingWindowLZ77_Mocks on Windows requires MSVC x64 or MinGW x64.")
endif()

if(WIN32)
    enable_language(RC)
    set_property(GLOBAL APPEND PROPERTY JOB_POOLS mocks_deploy_pool=1)
endif()

find_package(Qt6 6.12 QUIET COMPONENTS LinguistTools)

if(WIN32)
    if(MSVC)
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "msvc")
    elseif(MINGW)
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "mingw")
    else()
        set(MOCKS_DETECTED_TOOLCHAIN_SUFFIX "windows")
    endif()
    set(MOCKS_DEFAULT_EXPORT_DIR "D:/MISCELANEAS/VSCode/SlidingWindowLZ77/mocks_${MOCKS_DETECTED_TOOLCHAIN_SUFFIX}")
else()
    set(MOCKS_DEFAULT_EXPORT_DIR "")
endif()

if(WIN32)
    set(MOCKS_WINDOWS_ICON_NAME "lz77.ico")
    set(MOCKS_WINDOWS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icones/${MOCKS_WINDOWS_ICON_NAME}")
    if(NOT EXISTS "${MOCKS_WINDOWS_ICON}")
        set(MOCKS_WINDOWS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/../source/assets/icons/${MOCKS_WINDOWS_ICON_NAME}")
    endif()
endif()

set(MOCKS_ICON_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/icones/lz77.ico"
    "${CMAKE_CURRENT_SOURCE_DIR}/icones/lz77.png"
)

set(
    MOCKS_EXPORT_DIR
    "${MOCKS_DEFAULT_EXPORT_DIR}"
    CACHE PATH
    "Additional directory that receives the deployed mocks runtime"
)

if(MOCKS_EXPORT_DIR)
    file(TO_CMAKE_PATH "${MOCKS_EXPORT_DIR}" MOCKS_EXPORT_DIR_NORMALIZED)
else()
    set(MOCKS_EXPORT_DIR_NORMALIZED "")
endif()

function(mocks_add_windows_icon TARGET_NAME)
    if(NOT WIN32)
        return()
    endif()

    if(NOT EXISTS "${MOCKS_WINDOWS_ICON}")
        message(WARNING "Mock icon was not found at ${MOCKS_WINDOWS_ICON}. ${TARGET_NAME} will be built without the requested icon.")
        return()
    endif()

    file(TO_NATIVE_PATH "${MOCKS_WINDOWS_ICON}" LZ77_APP_ICON_NATIVE)
    string(REPLACE "\\" "\\\\" LZ77_APP_ICON_RC_PATH "${LZ77_APP_ICON_NATIVE}")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/../source/windows_app_icon.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_app_icon.rc"
        @ONLY
    )
    target_sources(
        "${TARGET_NAME}"
        PRIVATE
            "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}_app_icon.rc"
    )

    target_compile_definitions(
        "${TARGET_NAME}"
        PRIVATE
            "MOCKS_WINDOWS_ICON_NAME=\"${MOCKS_WINDOWS_ICON_NAME}\""
    )

    add_custom_command(
        TARGET "${TARGET_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/icones"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            ${MOCKS_ICON_FILES}
            "$<TARGET_FILE_DIR:${TARGET_NAME}>/icones"
        VERBATIM
    )
endfunction()

function(mocks_apply_common_options TARGET_NAME)
    set_target_properties(
        "${TARGET_NAME}"
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    target_include_directories(
        "${TARGET_NAME}"
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/.."
    )

    target_compile_definitions(
        "${TARGET_NAME}"
        PRIVATE
            $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
            $<$<CONFIG:Release>:QT_NO_INFO_OUTPUT>
            $<$<CONFIG:RelWithDebInfo>:QT_NO_DEBUG_OUTPUT>
            $<$<CONFIG:RelWithDebInfo>:QT_NO_INFO_OUTPUT>
    )

    if(WIN32)
        set_property(TARGET "${TARGET_NAME}" PROPERTY JOB_POOL_LINK mocks_deploy_pool)
    endif()

    if(MSVC)
        set_property(TARGET "${TARGET_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
                $<$<CONFIG:Release>:/O2>
                $<$<CONFIG:Release>:/Oi>
                $<$<CONFIG:Release>:/Ot>
                $<$<CONFIG:Release>:/fp:fast>
                $<$<CONFIG:RelWithDebInfo>:/O2>
                $<$<CONFIG:RelWithDebInfo>:/Oi>
                $<$<CONFIG:RelWithDebInfo>:/Ot>
                $<$<CONFIG:RelWithDebInfo>:/fp:fast>
        )
    elseif(MINGW)
        target_compile_options(
            "${TARGET_NAME}"
            PRIVATE
                $<$<CONFIG:Release>:-O3>
                $<$<CONFIG:Release>:-ffunction-sections>
                $<$<CONFIG:Release>:-fdata-sections>
                $<$<CONFIG:Release>:-fomit-frame-pointer>
                $<$<CONFIG:RelWithDebInfo>:-O3>
                $<$<CONFIG:RelWithDebInfo>:-ffunction-sections>
                $<$<CONFIG:RelWithDebInfo>:-fdata-sections>
                $<$<CONFIG:RelWithDebInfo>:-fomit-frame-pointer>
        )
        target_link_options(
            "${TARGET_NAME}"
            PRIVATE
                $<$<CONFIG:Release>:-Wl,--gc-sections>
                $<$<CONFIG:RelWithDebInfo>:-Wl,--gc-sections>
        )
    endif()
endfunction()

function(mocks_add_deploy_target TARGET_NAME)
    if(NOT WIN32)
        return()
    endif()

    set(_qmake_executable "")
    foreach(_qt_loc_prop IN ITEMS
        IMPORTED_LOCATION_RELEASE
        IMPORTED_LOCATION_RELWITHDEBINFO
        IMPORTED_LOCATION
        IMPORTED_LOCATION_DEBUG
    )
        get_target_property(_qt_loc_candidate Qt6::qmake ${_qt_loc_prop})
        if(_qt_loc_candidate AND NOT _qt_loc_candidate MATCHES "-NOTFOUND$" AND EXISTS "${_qt_loc_candidate}")
            set(_qmake_executable "${_qt_loc_candidate}")
            break()
        endif()
    endforeach()

    if(_qmake_executable)
        get_filename_component(MOCKS_QT_PREFIX_DIR "${_qmake_executable}" DIRECTORY)
    else()
        get_filename_component(MOCKS_QT_PREFIX_DIR "${Qt6_DIR}/../../../bin" ABSOLUTE)
    endif()

    find_program(MOCKS_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${MOCKS_QT_PREFIX_DIR}"
        NO_DEFAULT_PATH
    )
    if(NOT MOCKS_WINDEPLOYQT_EXECUTABLE)
        find_program(MOCKS_WINDEPLOYQT_EXECUTABLE
            NAMES windeployqt windeployqt6
            HINTS "${MOCKS_QT_PREFIX_DIR}"
        )
    endif()

    if(NOT MOCKS_WINDEPLOYQT_EXECUTABLE)
        message(WARNING "windeployqt.exe was not found. ${TARGET_NAME} will not be deployed automatically.")
        return()
    endif()

    set(_mocks_toolchain_family "")
    set(MOCKS_MSVC_REDIST_DIR "")
    set(_mocks_mingw_bin_dir "")

    if(MSVC)
        set(_mocks_toolchain_family "MSVC")
        foreach(_mocks_redist_glob IN ITEMS
            "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC/14.52.37025/x64/Microsoft.VC145.CRT"
            "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.52.37025/x64/Microsoft.VC145.CRT"
            "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC/*/x64/Microsoft.VC145.CRT"
            "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/*/x64/Microsoft.VC145.CRT"
            "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC/*/x64/Microsoft.VC*.CRT"
            "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/*/x64/Microsoft.VC*.CRT"
        )
            file(GLOB _mocks_redist_candidates CONFIGURE_DEPENDS LIST_DIRECTORIES true "${_mocks_redist_glob}")
            if(_mocks_redist_candidates)
                list(GET _mocks_redist_candidates 0 MOCKS_MSVC_REDIST_DIR)
                break()
            endif()
        endforeach()
    elseif(MINGW)
        set(_mocks_toolchain_family "MINGW")
        if(CMAKE_CXX_COMPILER)
            get_filename_component(_mocks_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        endif()
    endif()

    set(MOCKS_DEPLOY_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/deploy_optional_qt_binary.cmake")
    set(MOCKS_DEPLOY_LOCK_FILE "${CMAKE_BINARY_DIR}/mocks_deploy.lock")

    add_custom_command(
        TARGET "${TARGET_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}"
            "-DWINDEPLOYQT_EXECUTABLE=${MOCKS_WINDEPLOYQT_EXECUTABLE}"
            "-DTARGET_FILE=$<TARGET_FILE:${TARGET_NAME}>"
            "-DDEPLOY_CONFIG=$<CONFIG>"
            "-DTOOLCHAIN_FAMILY=${_mocks_toolchain_family}"
            "-DMSVC_REDIST_DIR=${MOCKS_MSVC_REDIST_DIR}"
            "-DMINGW_BIN_DIR=${_mocks_mingw_bin_dir}"
            "-DLOCK_FILE=${MOCKS_DEPLOY_LOCK_FILE}"
            "-DCLEAN_STALE_LAYOUT=ON"
            -P "${MOCKS_DEPLOY_SCRIPT}"
        COMMENT "Deploying ${TARGET_NAME} minimal runtime to local build directory"
        VERBATIM
    )

    if(MOCKS_EXPORT_DIR_NORMALIZED)
        add_custom_command(
            TARGET "${TARGET_NAME}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${MOCKS_EXPORT_DIR_NORMALIZED}/icones"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                ${MOCKS_ICON_FILES}
                "${MOCKS_EXPORT_DIR_NORMALIZED}/icones"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:${TARGET_NAME}>"
                "${MOCKS_EXPORT_DIR_NORMALIZED}/"
            COMMAND "${CMAKE_COMMAND}"
                "-DWINDEPLOYQT_EXECUTABLE=${MOCKS_WINDEPLOYQT_EXECUTABLE}"
                "-DTARGET_FILE=${MOCKS_EXPORT_DIR_NORMALIZED}/$<TARGET_FILE_NAME:${TARGET_NAME}>"
                "-DDEPLOY_CONFIG=$<CONFIG>"
                "-DTOOLCHAIN_FAMILY=${_mocks_toolchain_family}"
                "-DMSVC_REDIST_DIR=${MOCKS_MSVC_REDIST_DIR}"
                "-DMINGW_BIN_DIR=${_mocks_mingw_bin_dir}"
                "-DLOCK_FILE=${MOCKS_DEPLOY_LOCK_FILE}"
                "-DCLEAN_STALE_LAYOUT=ON"
                -P "${MOCKS_DEPLOY_SCRIPT}"
            COMMENT "Deploying ${TARGET_NAME} minimal runtime to external export directory"
            VERBATIM
        )
    endif()

    add_custom_target(
        "${TARGET_NAME}Deploy"
        ALL
        DEPENDS "${TARGET_NAME}"
    )

    if(MOCKS_LAST_DEPLOY_TARGET)
        add_dependencies("${TARGET_NAME}Deploy" "${MOCKS_LAST_DEPLOY_TARGET}")
    endif()
    set(MOCKS_LAST_DEPLOY_TARGET "${TARGET_NAME}Deploy" PARENT_SCOPE)
endfunction()

function(mocks_add_lrelease_target TARGET_NAME)
    if(NOT TARGET Qt6::lrelease)
        message(WARNING "Qt6 LinguistTools/lrelease was not found. ${TARGET_NAME} will not be available.")
        return()
    endif()

    set(MOCKS_TRANSLATION_DIRS
        "source/locale/translations"
    )
    set(MOCKS_QM_OUTPUTS)

    foreach(MOCKS_TRANSLATION_DIR IN LISTS MOCKS_TRANSLATION_DIRS)
        file(
            GLOB
            MOCKS_TS_FILES
            CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/../${MOCKS_TRANSLATION_DIR}/*.ts"
        )

        foreach(MOCKS_TS_FILE IN LISTS MOCKS_TS_FILES)
            get_filename_component(MOCKS_TS_BASENAME "${MOCKS_TS_FILE}" NAME_WE)
            get_filename_component(MOCKS_TS_DIR "${MOCKS_TS_FILE}" DIRECTORY)
            set(MOCKS_QM_FILE "${MOCKS_TS_DIR}/${MOCKS_TS_BASENAME}.qm")
            add_custom_command(
                OUTPUT "${MOCKS_QM_FILE}"
                COMMAND Qt6::lrelease
                    "${MOCKS_TS_FILE}"
                    -qm
                    "${MOCKS_QM_FILE}"
                DEPENDS "${MOCKS_TS_FILE}"
                COMMENT "Compiling ${MOCKS_TS_FILE} to ${MOCKS_QM_FILE}"
                VERBATIM
            )
            list(APPEND MOCKS_QM_OUTPUTS "${MOCKS_QM_FILE}")
        endforeach()
    endforeach()

    if(MOCKS_QM_OUTPUTS)
        add_custom_target("${TARGET_NAME}" DEPENDS ${MOCKS_QM_OUTPUTS})
    endif()
endfunction()

function(mocks_add_gui_run_target TARGET_NAME TOOL_TARGET)
    if(NOT WIN32)
        return()
    endif()

    add_custom_target(
        "${TARGET_NAME}"
        DEPENDS "${TOOL_TARGET}Deploy"
        COMMAND "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=$<TARGET_FILE_DIR:${TOOL_TARGET}>"
            "QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:${TOOL_TARGET}>/platforms"
            cmd /c start "" /b "$<TARGET_FILE:${TOOL_TARGET}>" ${ARGN}
        VERBATIM
        USES_TERMINAL
    )
endfunction()

function(mocks_add_terminal_run_target TARGET_NAME TOOL_TARGET)
    if(NOT WIN32)
        return()
    endif()

    add_custom_target(
        "${TARGET_NAME}"
        DEPENDS "${TOOL_TARGET}Deploy"
        COMMAND "${CMAKE_COMMAND}" -E chdir "$<TARGET_FILE_DIR:${TOOL_TARGET}>"
            "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=$<TARGET_FILE_DIR:${TOOL_TARGET}>"
            "QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:${TOOL_TARGET}>/platforms"
            "$<TARGET_FILE:${TOOL_TARGET}>"
            ${ARGN}
        VERBATIM
        USES_TERMINAL
    )
endfunction()

add_executable(
    CompileTranslationsGUI
    WIN32
    CompileTranslationsGUI.cpp
    CompileTranslationsGUI.hpp
)
target_link_libraries(CompileTranslationsGUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
mocks_add_windows_icon(CompileTranslationsGUI)
mocks_apply_common_options(CompileTranslationsGUI)
mocks_add_deploy_target(CompileTranslationsGUI)

add_executable(
    CompileTranslationsTerminal
    CompileTranslationsTerminal.cpp
    CompileTranslationsTerminal.hpp
)
target_link_libraries(CompileTranslationsTerminal PRIVATE Qt6::Core)
if(WIN32)
    target_link_libraries(CompileTranslationsTerminal PRIVATE User32)
endif()
mocks_add_windows_icon(CompileTranslationsTerminal)
mocks_apply_common_options(CompileTranslationsTerminal)
mocks_add_deploy_target(CompileTranslationsTerminal)

mocks_add_lrelease_target(CompileTranslationsQm)

add_executable(
    find_VersionEditor
    WIN32
    find_VersionEditor.cpp
    find_VersionEditor.hpp
)
target_link_libraries(find_VersionEditor PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
target_compile_features(find_VersionEditor PRIVATE cxx_std_23)
mocks_add_windows_icon(find_VersionEditor)
mocks_apply_common_options(find_VersionEditor)
mocks_add_deploy_target(find_VersionEditor)

if(WIN32)
    set(MOCKS_DETACHED_GUI_LAUNCHER "${CMAKE_CURRENT_SOURCE_DIR}/launch_detached_gui.cmake")

    add_custom_target(
        MocksRun
        DEPENDS
            CompileTranslationsGUIDeploy
            CompileTranslationsTerminalDeploy
            find_VersionEditorDeploy
        COMMAND "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=$<TARGET_FILE_DIR:find_VersionEditor>"
            "QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:find_VersionEditor>/platforms"
            "${CMAKE_COMMAND}"
            -DLAUNCH_FILE=$<TARGET_FILE:find_VersionEditor>
            -DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:find_VersionEditor>
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        COMMAND "${CMAKE_COMMAND}" -E env
            "QT_PLUGIN_PATH=$<TARGET_FILE_DIR:CompileTranslationsGUI>"
            "QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:CompileTranslationsGUI>/platforms"
            "${CMAKE_COMMAND}"
            -DLAUNCH_FILE=$<TARGET_FILE:CompileTranslationsGUI>
            -DLAUNCH_WORKING_DIR=$<TARGET_FILE_DIR:CompileTranslationsGUI>
            -P "${MOCKS_DETACHED_GUI_LAUNCHER}"
        COMMENT "Launching mocks runtime tools"
        VERBATIM
        USES_TERMINAL
    )

    mocks_add_gui_run_target(find_VersionEditorRun find_VersionEditor)
    mocks_add_gui_run_target(CompileTranslationsGUIRunSlidingWindow CompileTranslationsGUI --module slidingwindowlz77)
    mocks_add_terminal_run_target(CompileTranslationsTerminalRunSlidingWindow CompileTranslationsTerminal --module slidingwindowlz77)
endif()
