cmake_minimum_required(VERSION 3.16)
project(AudioAnalyser)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    list(APPEND CMAKE_INSTALL_RPATH "/opt/homebrew/lib")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(GTest REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets Charts Multimedia Concurrent REQUIRED)
find_package(benchmark REQUIRED)

file(GLOB_RECURSE GUI_SOURCES "src/gui/*.cpp")
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES "src/utils/*.cpp")

set(LIB_SOURCES ${CORE_SOURCES} ${UTILS_SOURCES} ${GUI_SOURCES})

add_executable(main src/main.cpp ${LIB_SOURCES})
target_link_libraries(main PRIVATE 
    Qt6::Widgets 
    Qt6::Charts 
    Qt6::Multimedia 
    Qt6::Concurrent
)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "tests/unit_tests/*.cpp")
file(GLOB_RECURSE BENCH_SOURCES "tests/benchmarks/*.cpp")

add_executable(unit-tests tests/unit_tests/main.cpp ${UTILS_SOURCES} ${CORE_SOURCES} ${TEST_SOURCES})
target_link_libraries(unit-tests  PRIVATE 
    GTest::gtest_main 
)

add_executable(benchmark-tests tests/benchmarks/main.cpp ${CORE_SOURCES} ${BENCH_SOURCES})
target_link_libraries(benchmark-tests PRIVATE 
    benchmark::benchmark
)

add_test(NAME unit-tests COMMAND unit-tests)
set_tests_properties(unit-tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_test(NAME benchmark-tests COMMAND benchmark-tests)
set_tests_properties(benchmark-tests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)