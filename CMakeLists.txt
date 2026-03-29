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

set(GTEST_MSVC_RUNTIME_LIBRARY "static")
find_package(GTest REQUIRED)

find_package(Qt6 COMPONENTS Core Widgets Charts REQUIRED)

file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_executable(main src/main.cpp ${LIB_SOURCES})
target_link_libraries(main PRIVATE Qt6::Widgets Qt6::Charts)

enable_testing()
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
add_executable(tests ${TEST_SOURCES} ${LIB_SOURCES})

target_link_libraries(tests PRIVATE GTest::gtest_main Qt6::Widgets Qt6::Charts)