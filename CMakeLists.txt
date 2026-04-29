cmake_minimum_required(VERSION 3.24)
project(icepanic VERSION 0.9.1 LANGUAGES C)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT icepanic_sdl)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(ICEPANIC_BUILD_TESTS "Build core gameplay tests" ON)

include(FetchContent)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.10
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL2)

set(ICEPANIC_CORE_SOURCES
    src/core/game.c
)

add_library(icepanic_core STATIC ${ICEPANIC_CORE_SOURCES})
target_include_directories(icepanic_core PUBLIC src/core)
target_compile_definitions(icepanic_core PRIVATE _CRT_SECURE_NO_WARNINGS)

if(MSVC)
    target_compile_options(icepanic_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(icepanic_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(icepanic_sdl
    src/platform_sdl/main.c
)
if(TARGET SDL2::SDL2-static)
    set(ICEPANIC_SDL2_TARGET SDL2::SDL2-static)
else()
    set(ICEPANIC_SDL2_TARGET SDL2::SDL2)
endif()

target_link_libraries(icepanic_sdl PRIVATE icepanic_core ${ICEPANIC_SDL2_TARGET})
if(TARGET SDL2::SDL2main)
    target_link_libraries(icepanic_sdl PRIVATE SDL2::SDL2main)
endif()
target_compile_definitions(icepanic_sdl PRIVATE _CRT_SECURE_NO_WARNINGS)

if(MSVC)
    target_compile_options(icepanic_sdl PRIVATE /W4 /permissive-)
else()
    target_compile_options(icepanic_sdl PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(icepanic_iab_loader_demo
    src/tools/iab_loader_demo.c
)
target_compile_definitions(icepanic_iab_loader_demo PRIVATE _CRT_SECURE_NO_WARNINGS)
if(MSVC)
    target_compile_options(icepanic_iab_loader_demo PRIVATE /W4 /permissive-)
else()
    target_compile_options(icepanic_iab_loader_demo PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(icepanic_fun_agent
    src/tools/fun_agent.c
)
target_link_libraries(icepanic_fun_agent PRIVATE icepanic_core)
target_compile_definitions(icepanic_fun_agent PRIVATE _CRT_SECURE_NO_WARNINGS)
if(MSVC)
    target_compile_options(icepanic_fun_agent PRIVATE /W4 /permissive-)
else()
    target_compile_options(icepanic_fun_agent PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(ICEPANIC_BUILD_TESTS)
    include(CTest)
    enable_testing()

    add_executable(icepanic_core_tests
        tests/test_core.c
    )
    target_link_libraries(icepanic_core_tests PRIVATE icepanic_core)
    target_compile_definitions(icepanic_core_tests PRIVATE _CRT_SECURE_NO_WARNINGS)

    if(MSVC)
        target_compile_options(icepanic_core_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(icepanic_core_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME core_tests COMMAND icepanic_core_tests)
endif()
