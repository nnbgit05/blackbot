cmake_minimum_required(VERSION 3.20)

project(BlueBot2
    VERSION 0.0.1
    LANGUAGES CXX
)

add_executable(engine)

target_sources(engine PRIVATE
    src/board.cpp
    src/debug.cpp
    src/evaluation.cpp
    src/kt.cpp
    src/main.cpp
    src/move_ordering.cpp
    src/search.cpp
    src/tt.cpp
)

target_include_directories(engine PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party
)

target_compile_features(engine PRIVATE cxx_std_20)

if (MSVC)
    target_compile_options(engine PRIVATE
        /W4
        /permissive-
        $<$<CONFIG:Release>:/O2>
    )
else()
    target_compile_options(engine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
    )

    target_link_options(engine PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
    )
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

if(ipo_supported)
    set_property(TARGET engine PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
else()
    message(STATUS "IPO/LTO not supported: ${ipo_error}")
endif()
