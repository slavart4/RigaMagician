cmake_minimum_required(VERSION 4.1)
project(RigaMagician)

set(CMAKE_CXX_STANDARD 20)

add_executable(RigaMagician src/main.cpp
        src/Position.cpp
        src/Position.h
        src/Types.h
        src/Bitboards.cpp
        src/Bitboards.h
        src/BitboardUtils.cpp
        src/BitboardUtils.h
        src/Move.cpp
        src/Move.h
        src/Attacks.cpp
        src/Attacks.h
)
