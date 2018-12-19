# Name of project and executable
project(Connect4)

# set cmake version
cmake_minimum_required(VERSION 2.8)

# activate latest c++ compiler version
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# set flags to configure the warning settings
# Note: warnings are compiler specific
if( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# set build type to Debug/Release
set(CMAKE_BUILD_TYPE "Debug")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

# Populate a CMake variable with the sources
set(APP_SOURCES
    src/ui/form.h
    src/ui/form.cpp
    src/ui/form.ui
    src/logic/board.h
    src/logic/board.cpp
    src/logic/game.h
    src/logic/game.cpp
    src/logic/ai.h
    src/logic/ai.cpp
    src/utils/observer.h
    src/main.cpp
    src/icon/connect4.rc
)

set(APP_INCLUDE_DIRS
    ui
    logic
    utils
    icon
    src/ui
    src/logic
    src/utils
    src/icon
)
INCLUDE_DIRECTORIES(${APP_INCLUDE_DIRS})

find_package(Qt5Core)
find_package(Threads)

# Add an executable to the project and sources
add_executable(${PROJECT_NAME} ${APP_SOURCES})
# Use the Widgets module from Qt 5
target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(${PROJECT_NAME} Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT})


