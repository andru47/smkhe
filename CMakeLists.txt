cmake_minimum_required(VERSION 3.10)
project(smkhe)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/encoder.cpp src/plaintext.cpp src/parameters.cpp src/public_key.cpp src/secret_key.cpp src/encryptor.cpp src/math/ntt_transformer.cpp include/smkhe/math/ntt_transformer.h)
set(TEST_FILES test/encoder.cpp test/math/ntt_transformer.cpp)

add_library(smkhe SHARED ${SOURCE_FILES})

target_include_directories(smkhe PUBLIC ./include)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests ${TEST_FILES})
target_link_libraries(tests gtest_main smkhe)

include(GoogleTest)
gtest_discover_tests(tests)
