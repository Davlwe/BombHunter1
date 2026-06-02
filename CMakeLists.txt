cmake_minimum_required(VERSION 3.15)
project(BombHunter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# Options
# =========================
option(USE_SYSTEM_RAYLIB "Use system-installed raylib" OFF)
option(WEB_BUILD          "Build for web browser (requires Emscripten)" OFF)

# =========================
# Raylib
# =========================
if(WEB_BUILD)
    # ── Web build: raylib is compiled with emcc via FetchContent ──
    if(NOT EMSCRIPTEN)
        message(FATAL_ERROR
            "Emscripten not detected.\n"
            "Pass the toolchain file:\n"
            "  cmake -B build-web -DWEB_BUILD=ON \\\n"
            "    -DCMAKE_TOOLCHAIN_FILE=/opt/homebrew/opt/emscripten/libexec/cmake/Modules/Platform/Emscripten.cmake\n"
            "Or run:  ./web-build.sh")
    endif()

    set(PLATFORM Web CACHE STRING "Raylib platform" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.5
        GIT_SHALLOW    TRUE
    )
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)

elseif(USE_SYSTEM_RAYLIB)
    find_package(raylib REQUIRED)

else()
    # ── Native build: fetch raylib from source ──
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.5
        GIT_SHALLOW    TRUE
    )
    set(BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
endif()

# =========================
# Source files
# =========================
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

# =========================
# Link raylib
# =========================
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# =========================
# Platform-specific settings
# =========================
if(WEB_BUILD)
    # ── Emscripten / Web ──
    # SHELL: prefix prevents CMake from de-duplicating/stripping -s flags
    target_link_options(${PROJECT_NAME} PRIVATE
        "SHELL:-s USE_GLFW=3"
        "SHELL:-s ASYNCIFY"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s TOTAL_STACK=5MB"
        "SHELL:--embed-file ${CMAKE_SOURCE_DIR}/levels@/levels"
        "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/src/shell.html"
    )
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
        "-framework CoreGraphics"
    )

elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_DL_LIBS}
        pthread
    )
elseif(WIN32)
    # Windows libraries are handled by raylib's CMake config
endif()

# =========================
# Copy assets next to the executable (native builds only)
# =========================
if(NOT WEB_BUILD)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/levels"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/levels"
        COMMENT "Copying level files to build directory"
    )
endif()
