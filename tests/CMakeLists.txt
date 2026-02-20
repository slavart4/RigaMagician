file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

if(TEST_SOURCES)
    include(FetchContent)

    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(RigaMagicianTests
            ${TEST_SOURCES}
    )

    target_link_libraries(RigaMagicianTests
            PRIVATE
            RigaMagicianLib
            GTest::gtest
            GTest::gmock
    )

    add_test(NAME RigaMagicianTests COMMAND RigaMagicianTests)
else()
    message(WARNING "No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}. Skipping test target.")
endif()
