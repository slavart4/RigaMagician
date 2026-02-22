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
        MoveGenerator.cpp
        MoveGenerator.h
        utils/FenUtils.cpp
        utils/FenUtils.h
        Evaluation.cpp
        Evaluation.h
        Search.cpp
        Search.h
        UCI.cpp
        UCI.h
)

target_include_directories(RigaMagicianLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(RigaMagician main.cpp)
target_link_libraries(RigaMagician PRIVATE RigaMagicianLib)

add_custom_command(TARGET RigaMagician POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:RigaMagician>
        ${CMAKE_SOURCE_DIR}/$<TARGET_FILE_NAME:RigaMagician>
        COMMENT "Copying RigaMagician executable to project root"
)
