cmake_minimum_required(VERSION 3.31)
project(agt LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(thirdparty/dwhbll)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glbinding REQUIRED)

pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-egl)
pkg_check_modules(XKB REQUIRED xkbcommon)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(FREETYPE REQUIRED freetype2)

# for some reason using pkg_get_variable doesn't work
execute_process(
    COMMAND pkg-config --variable=pkgdatadir wayland-protocols
    OUTPUT_VARIABLE WL_PROTOCOLS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(AGT_SOURCES
    src/event.cpp

    src/wl/wl.cpp
    src/wl/window.cpp
    # src/wl/input.cpp

    src/gl/gl.cpp
    src/gl/shaders.cpp
    src/gl/window.cpp

    src/ui/ui.cpp
    src/ui/draw.cpp
    src/ui/text.cpp

    src/ui/widgets/hbox.cpp
    src/ui/widgets/label.cpp
    src/ui/widgets/button.cpp
)

set(XMLS
    ${WL_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml
    # need tablet for cursor-shape
    ${WL_PROTOCOLS_DIR}/stable/tablet/tablet-v2.xml
    ${WL_PROTOCOLS_DIR}/staging/cursor-shape/cursor-shape-v1.xml
)

foreach(XML ${XMLS})
    get_filename_component(BASENAME ${XML} NAME_WE)

    set(C_GEN ${BASENAME}-protocol.c)
    set(H_GEN ${BASENAME}-client-protocol.h)

    add_custom_command(
        OUTPUT ${C_GEN} ${H_GEN}
        COMMAND wayland-scanner private-code ${XML} ${C_GEN}
        COMMAND wayland-scanner client-header ${XML} ${H_GEN}
        COMMENT "Generating protocol code for ${BASENAME}"
        VERBATIM
    )

    list(APPEND AGT_SOURCES ${C_GEN} ${H_GEN})
endforeach()

add_library(agt SHARED ${AGT_SOURCES})

target_include_directories(agt PUBLIC
    include
    thirdparty
    ${CMAKE_CURRENT_BINARY_DIR} # for wayland files
    ${WAYLAND_INCLUDE_DIRS}
    ${XKB_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

target_link_libraries(agt PUBLIC
    glbinding::glbinding
    dwhbll
    OpenGL::OpenGL
    glm::glm
    ${WAYLAND_LIBRARIES}
    ${XKB_LIBRARIES}
    ${EGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

set(AGT_TEST_SOURCES
    tests/main.cpp
)

add_executable(agt_test ${AGT_TEST_SOURCES})
target_link_libraries(agt_test agt)
