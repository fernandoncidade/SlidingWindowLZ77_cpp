cmake_minimum_required(VERSION 3.28)
project(LZ77Visualizer VERSION 1.0 LANGUAGES C CXX)

if(WIN32 AND NOT MSVC AND NOT MINGW)
    message(FATAL_ERROR "LZ77Visualizer on Windows requires MSVC x64 or MinGW x64.")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.12 REQUIRED COMPONENTS Core Gui Widgets LinguistTools)

option(BUILD_MOCKS "Build utility and translation tools from mocks" OFF)

if(BUILD_MOCKS)
    add_subdirectory(mocks)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/lzma_sdk)

set(LZMA_SDK_SOURCES
    source/lzma_sdk/7zAlloc.c
    source/lzma_sdk/7zArcIn.c
    source/lzma_sdk/7zBuf.c
    source/lzma_sdk/7zBuf2.c
    source/lzma_sdk/7zCrc.c
    source/lzma_sdk/7zCrcOpt.c
    source/lzma_sdk/7zDec.c
    source/lzma_sdk/7zFile.c
    source/lzma_sdk/7zStream.c
    source/lzma_sdk/Bcj2.c
    source/lzma_sdk/Bra.c
    source/lzma_sdk/Bra86.c
    source/lzma_sdk/BraIA64.c
    source/lzma_sdk/CpuArch.c
    source/lzma_sdk/Delta.c
    source/lzma_sdk/LzmaDec.c
    source/lzma_sdk/Lzma2Dec.c
    source/lzma_sdk/Ppmd7.c
    source/lzma_sdk/Ppmd7Dec.c
)

if(WIN32)
    enable_language(RC)
endif()

set(APP_SOURCES
    main.cpp
    source/interface/mainwindow.cpp
    source/interface/mainwindow.hpp
    source/interface/mainwindow.h
    source/interface/LzmaMatchesTableModel.cpp
    source/interface/LzmaMatchesTableModel.hpp
    source/interface/DocumentViewerDialog.cpp
    source/interface/DocumentViewerDialog.hpp
    source/interface/DocumentationDialog.cpp
    source/interface/DocumentationDialog.hpp
    source/interface/AppIconHelper.hpp
    source/config/LzmaExtractor.cpp
    source/config/LzmaExtractor.hpp
    source/locale/tr_01_gerenciadorTraducao.cpp
    source/locale/tr_01_gerenciadorTraducao.hpp
    source/logs/LogManager.cpp
    source/logs/LogManager.hpp
)

set(TS_FILES
    source/locale/translations/slidingwindowlz77_pt_BR.ts
    source/locale/translations/slidingwindowlz77_en_US.ts
)

qt6_add_translation(QM_FILES ${TS_FILES})

set(APP_EXTRA_SOURCES)
set(APP_EXPORT_ROOT_DIR "D:/MISCELANEAS/VSCode/SlidingWindowLZ77")
set(APP_EXPORT_DIR_BASENAME "SlidingWindowLZ77_cpp")

if(WIN32)
    if(MSVC)
        set(LZ77_DETECTED_TOOLCHAIN_SUFFIX "msvc")
    elseif(MINGW)
        set(LZ77_DETECTED_TOOLCHAIN_SUFFIX "mingw")
    else()
        set(LZ77_DETECTED_TOOLCHAIN_SUFFIX "windows")
    endif()
    set(LZ77_DEFAULT_EXPORT_DIR "${APP_EXPORT_ROOT_DIR}/${APP_EXPORT_DIR_BASENAME}_${LZ77_DETECTED_TOOLCHAIN_SUFFIX}")
else()
    set(LZ77_DEFAULT_EXPORT_DIR "")
endif()

set(
    LZ77_EXPORT_DIR
    "${LZ77_DEFAULT_EXPORT_DIR}"
    CACHE PATH
    "Additional external directory that receives the deployed application runtime"
)

if(LZ77_EXPORT_DIR)
    file(TO_CMAKE_PATH "${LZ77_EXPORT_DIR}" LZ77_EXPORT_DIR_NORMALIZED)
else()
    set(LZ77_EXPORT_DIR_NORMALIZED "")
endif()

if(WIN32)
    set(APP_WINDOWS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icons/lz77.ico")
    if(EXISTS "${APP_WINDOWS_ICON}")
        file(TO_NATIVE_PATH "${APP_WINDOWS_ICON}" APP_ICON_NATIVE)
        string(REPLACE "\\" "\\\\" LZ77_APP_ICON_RC_PATH "${APP_ICON_NATIVE}")
        configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/source/windows_app_icon.rc.in"
            "${CMAKE_CURRENT_BINARY_DIR}/lz77_app_icon.rc"
            @ONLY
        )
        list(APPEND APP_EXTRA_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/lz77_app_icon.rc")
    endif()
endif()

add_executable(LZ77Visualizer
    WIN32
    ${APP_SOURCES}
    ${LZMA_SDK_SOURCES}
    ${QM_FILES}
    ${APP_EXTRA_SOURCES}
)

set_target_properties(LZ77Visualizer PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(LZ77Visualizer PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

if(MSVC)
    set_property(TARGET LZ77Visualizer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
elseif(MINGW)
    target_compile_options(LZ77Visualizer PRIVATE
        $<$<CONFIG:Release>:-ffunction-sections>
        $<$<CONFIG:Release>:-fdata-sections>
        $<$<CONFIG:RelWithDebInfo>:-ffunction-sections>
        $<$<CONFIG:RelWithDebInfo>:-fdata-sections>
    )
    target_link_options(LZ77Visualizer PRIVATE
        $<$<CONFIG:Release>:-Wl,--gc-sections>
        $<$<CONFIG:RelWithDebInfo>:-Wl,--gc-sections>
    )
endif()

set(LZ77_RUNTIME_ICON_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icons/lz77.ico"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/icons/lz77.png"
)

set(LZ77_RUNTIME_LICENSE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/LICENSE"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/THIRD_PARTY_COMPONENT_NOTICES.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/OPEN_SOURCE_SOURCE_OFFER_pt_BR.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/OPEN_SOURCE_SOURCE_OFFER_en_US.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/GNU GENERAL PUBLIC LICENSE Version 3.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/BSD License Clause-3.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/GCC Runtime Library Exception 3.1.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/MinGW-w64 Runtime License.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/LICENSES/Microsoft Redistributable Runtime Notice.txt"
)

set(LZ77_RUNTIME_RELEASE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/RELEASES/release_pt-BR.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/assets/RELEASES/release_en-US.md"
)

set(LZ77_RUNTIME_DOCS_PT_BR
    "${CMAKE_CURRENT_SOURCE_DIR}/source/docs/pt_BR/LEIAME.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/docs/pt_BR/SOBRE.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/docs/pt_BR/MANUAL.md"
)

set(LZ77_RUNTIME_DOCS_EN_US
    "${CMAKE_CURRENT_SOURCE_DIR}/source/docs/en_US/README.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/docs/en_US/ABOUT.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/docs/en_US/MANUAL.md"
)

# Copy compiled .qm translations, icons, licenses, release notes, and localized docs to build dir
add_custom_command(TARGET LZ77Visualizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LZ77Visualizer>/translations"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${QM_FILES}
        "$<TARGET_FILE_DIR:LZ77Visualizer>/translations"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${QM_FILES}
        "${CMAKE_CURRENT_SOURCE_DIR}/source/locale/translations"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LZ77Visualizer>/assets/icons"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LZ77_RUNTIME_ICON_FILES}
        "$<TARGET_FILE_DIR:LZ77Visualizer>/assets/icons"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LZ77Visualizer>/assets/LICENSES"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LZ77_RUNTIME_LICENSE_FILES}
        "$<TARGET_FILE_DIR:LZ77Visualizer>/assets/LICENSES"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LZ77Visualizer>/assets/RELEASES"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LZ77_RUNTIME_RELEASE_FILES}
        "$<TARGET_FILE_DIR:LZ77Visualizer>/assets/RELEASES"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LZ77Visualizer>/docs/pt_BR"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LZ77_RUNTIME_DOCS_PT_BR}
        "$<TARGET_FILE_DIR:LZ77Visualizer>/docs/pt_BR"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:LZ77Visualizer>/docs/en_US"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LZ77_RUNTIME_DOCS_EN_US}
        "$<TARGET_FILE_DIR:LZ77Visualizer>/docs/en_US"
    COMMENT "Deploying minimal translations, icons, licenses, release notes, and documentation to build directory..."
    VERBATIM
)

if(WIN32)
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
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    else()
        get_filename_component(_qt_bin_dir "${Qt6_DIR}/../../../bin" ABSOLUTE)
    endif()
    get_filename_component(_qt_prefix_dir "${_qt_bin_dir}/.." ABSOLUTE)
    set(_qt_translations_dir "${_qt_prefix_dir}/translations")

    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${_qt_bin_dir}"
        NO_DEFAULT_PATH
    )
    if(NOT WINDEPLOYQT_EXECUTABLE)
        find_program(WINDEPLOYQT_EXECUTABLE
            NAMES windeployqt windeployqt6
            HINTS "${_qt_bin_dir}"
        )
    endif()

    set(_lz77_toolchain_family "")
    set(_lz77_msvc_redist_dir "")
    set(_lz77_mingw_bin_dir "")

    if(MSVC)
        set(_lz77_toolchain_family "MSVC")
        foreach(_msvc_redist_glob IN ITEMS
            "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC/14.52.37025/x64/Microsoft.VC145.CRT"
            "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/14.52.37025/x64/Microsoft.VC145.CRT"
            "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC/*/x64/Microsoft.VC145.CRT"
            "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/*/x64/Microsoft.VC145.CRT"
            "C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/Redist/MSVC/*/x64/Microsoft.VC*.CRT"
            "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Redist/MSVC/*/x64/Microsoft.VC*.CRT"
        )
            file(GLOB _msvc_dir_candidates CONFIGURE_DEPENDS LIST_DIRECTORIES true "${_msvc_redist_glob}")
            if(_msvc_dir_candidates)
                list(GET _msvc_dir_candidates 0 _lz77_msvc_redist_dir)
                break()
            endif()
        endforeach()
    elseif(MINGW)
        set(_lz77_toolchain_family "MINGW")
        if(CMAKE_CXX_COMPILER)
            get_filename_component(_lz77_mingw_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        endif()
    endif()

    set(LZ77_DEPLOY_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy_optional_qt_binary.cmake")
    set(LZ77_DEPLOY_LOCK_FILE "${CMAKE_BINARY_DIR}/lz77_deploy.lock")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET LZ77Visualizer POST_BUILD
            COMMAND "${CMAKE_COMMAND}"
                "-DWINDEPLOYQT_EXECUTABLE=${WINDEPLOYQT_EXECUTABLE}"
                "-DTARGET_FILE=$<TARGET_FILE:LZ77Visualizer>"
                "-DDEPLOY_CONFIG=$<CONFIG>"
                "-DTOOLCHAIN_FAMILY=${_lz77_toolchain_family}"
                "-DMSVC_REDIST_DIR=${_lz77_msvc_redist_dir}"
                "-DMINGW_BIN_DIR=${_lz77_mingw_bin_dir}"
                "-DQT_TRANSLATIONS_DIR=${_qt_translations_dir}"
                "-DLOCK_FILE=${LZ77_DEPLOY_LOCK_FILE}"
                "-DCLEAN_STALE_LAYOUT=ON"
                -P "${LZ77_DEPLOY_SCRIPT}"
            COMMENT "Deploying minimal Qt runtime dependencies next to $<TARGET_FILE_NAME:LZ77Visualizer>..."
            VERBATIM
        )
    endif()

    if(LZ77_EXPORT_DIR_NORMALIZED)
        add_custom_command(TARGET LZ77Visualizer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${LZ77_EXPORT_DIR_NORMALIZED}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:LZ77Visualizer>"
                "${LZ77_EXPORT_DIR_NORMALIZED}/"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${LZ77_EXPORT_DIR_NORMALIZED}/translations"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${QM_FILES}
                "${LZ77_EXPORT_DIR_NORMALIZED}/translations"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${LZ77_EXPORT_DIR_NORMALIZED}/assets/icons"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${LZ77_RUNTIME_ICON_FILES}
                "${LZ77_EXPORT_DIR_NORMALIZED}/assets/icons"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${LZ77_EXPORT_DIR_NORMALIZED}/assets/LICENSES"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${LZ77_RUNTIME_LICENSE_FILES}
                "${LZ77_EXPORT_DIR_NORMALIZED}/assets/LICENSES"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${LZ77_EXPORT_DIR_NORMALIZED}/assets/RELEASES"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${LZ77_RUNTIME_RELEASE_FILES}
                "${LZ77_EXPORT_DIR_NORMALIZED}/assets/RELEASES"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${LZ77_EXPORT_DIR_NORMALIZED}/docs/pt_BR"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${LZ77_RUNTIME_DOCS_PT_BR}
                "${LZ77_EXPORT_DIR_NORMALIZED}/docs/pt_BR"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${LZ77_EXPORT_DIR_NORMALIZED}/docs/en_US"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${LZ77_RUNTIME_DOCS_EN_US}
                "${LZ77_EXPORT_DIR_NORMALIZED}/docs/en_US"
            COMMENT "Deploying minimal translations, icons, licenses, release notes, and docs to external export directory..."
            VERBATIM
        )

        if(WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET LZ77Visualizer POST_BUILD
                COMMAND "${CMAKE_COMMAND}"
                    "-DWINDEPLOYQT_EXECUTABLE=${WINDEPLOYQT_EXECUTABLE}"
                    "-DTARGET_FILE=${LZ77_EXPORT_DIR_NORMALIZED}/$<TARGET_FILE_NAME:LZ77Visualizer>"
                    "-DDEPLOY_CONFIG=$<CONFIG>"
                    "-DTOOLCHAIN_FAMILY=${_lz77_toolchain_family}"
                    "-DMSVC_REDIST_DIR=${_lz77_msvc_redist_dir}"
                    "-DMINGW_BIN_DIR=${_lz77_mingw_bin_dir}"
                    "-DQT_TRANSLATIONS_DIR=${_qt_translations_dir}"
                    "-DLOCK_FILE=${LZ77_DEPLOY_LOCK_FILE}"
                    "-DCLEAN_STALE_LAYOUT=ON"
                    -P "${LZ77_DEPLOY_SCRIPT}"
                COMMENT "Deploying minimal Qt runtime dependencies to external export directory..."
                VERBATIM
            )
        endif()
    endif()
endif()
