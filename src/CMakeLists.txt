add_library(RigaMagicianLib
        Position.cpp
        Position.h
        Types.h
        Bitboards.cpp
        Bitboards.h
        utils/BitboardUtils.cpp
        utils/BitboardUtils.h
        Move.h
        Attacks.cpp
        Attacks.h
        utils/FenUtils.cpp
        utils/FenUtils.h
)

target_include_directories(RigaMagicianLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(RigaMagician main.cpp)
target_link_libraries(RigaMagician PRIVATE RigaMagicianLib)
