cmake_minimum_required(VERSION 3.13)
project(net_simulation)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors
        -Werror=switch)

include_directories(
        include
)


set(SOURCE_FILES
        src/factory.cpp src/helpers.cpp src/nodes.cpp src/package.cpp
        src/reports.cpp src/simulation.cpp src/storage_types.cpp)

set(TEST_FILES
        test/test_Factory.cpp test/test_factory_io.cpp test/test_nodes.cpp
        test/test_package.cpp test/test_reports.cpp test/test_simulate.cpp
        test/test_storage_types.cpp)


# == Unit testing using Google Testing Framework ==

add_executable(test_simulation ${SOURCE_FILES} ${TEST_FILES} test/main_gtest.cpp)

target_include_directories(test_simulation PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
        )

add_subdirectory(./googletest-master build)

target_link_libraries(test_simulation gmock)

target_compile_definitions(test_simulation PUBLIC EXERCISE_ID=SIMULATION)