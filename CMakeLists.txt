cmake_minimum_required(VERSION 3.18)

set(NETWORKGUILIBRARY_FILES src/BezierCurve.cpp src/InfoPane.cpp src/Interface.cpp src/NetworkTopology.cpp src/NetworkWindow.cpp src/Tests.cpp)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
# or -D CMAKE_BUILD_TYPE=Release

project(NetworkGUILibrary)

set (SFML_DIR "/usr/lib/x86_64-linux-gnu/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

if(AS_LIBRARY)
    add_library("${PROJECT_NAME}" ${NETWORKGUILIBRARY_FILES})
else()
    set(NETWORKGUILIBRARY_FILES ${NETWORKGUILIBRARY_FILES} src/main.cpp)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    add_executable("${PROJECT_NAME}" ${NETWORKGUILIBRARY_FILES})
endif()

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
