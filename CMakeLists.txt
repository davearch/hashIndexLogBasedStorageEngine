cmake_minimum_required(VERSION 3.24)
project(hashIndex)

set(CMAKE_CXX_STANDARD 20)

include_directories("include")

add_executable(hashIndex main.cpp LogFile.h LogFile.cpp BitcaskEntry.cpp BitcaskEntry.h include/json.hpp Datastore.cpp Datastore.h HintEntry.cpp HintEntry.h)
