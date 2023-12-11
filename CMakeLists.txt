cmake_minimum_required(VERSION 3.25)
project(2D_game_assignment)

set(CMAKE_CXX_STANDARD 14)

# Set the SFML_DIR variable to the path where the SFML CMake configuration is located
set(SFML_DIR "/usr/local/Cellar/sfml/2.6.1/lib/cmake/SFML")

# Find the SFML package
find_package(SFML 2.6.1 COMPONENTS graphics window system REQUIRED)

include_directories(include)

configure_file(resources/background.png ${CMAKE_CURRENT_BINARY_DIR}/resources/background.png COPYONLY)
configure_file(resources/pacman.png ${CMAKE_CURRENT_BINARY_DIR}/resources/pacman.png COPYONLY)

add_executable(2D_game_assignment
        src/main.cpp
        src/game.cpp)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# Test suite sources
set(TEST_SOURCES
        src/move.cpp
        test/CoordinateTest.cpp
        )

# Add the Google Test framework
find_package(GTest REQUIRED)

# Add the test executable
add_executable(CoordinateTest ${TEST_SOURCES})

# Link the test executable with the main module and Google Test
target_link_libraries(CoordinateTest PRIVATE GTest::GTest GTest::Main)

# Specify the executable output directory
set_target_properties(CoordinateTest PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
        )

# Add a test target for running tests using ctest
add_test(NAME CoordinateTest COMMAND CoordinateTest)
