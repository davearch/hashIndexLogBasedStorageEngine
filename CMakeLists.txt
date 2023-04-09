cmake_minimum_required(VERSION 3.24)
project(hashIndex)

set(CMAKE_CXX_STANDARD 20)

include_directories("include")

add_executable(hashIndex main.cpp LogFile.h LogFile.cpp BitcaskEntry.cpp BitcaskEntry.h include/json.hpp Datastore.cpp Datastore.h HintEntry.cpp HintEntry.h)

# Create a symbolic link to the logs folder in the build directory
if (WIN32)
    add_custom_command(
            TARGET hashIndex POST_BUILD
            COMMAND cmd.exe /C "mklink /D logs ..\\logs || exit 0"
            COMMENT "Creating symbolic link to logs folder"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
else()
    add_custom_command(
            TARGET hashIndex POST_BUILD
            COMMAND ln -sf ${CMAKE_SOURCE_DIR}/logs ${CMAKE_BINARY_DIR}/logs
            COMMENT "Creating symbolic link to logs folder"
    )
endif()