cmake_minimum_required(VERSION 3.10)
project(smkhe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH})

set(SOURCE_FILES src/encoder.cpp src/plaintext.cpp src/parameters.cpp)
set(TEST_FILES test/encoder.cpp)

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(FLINT REQUIRED)
find_package(GMP REQUIRED)

add_library(smkhe SHARED ${SOURCE_FILES})

target_link_libraries(smkhe FLINT GMP::gmp Eigen3::Eigen)

target_include_directories(smkhe PUBLIC ./include ${GMP_INCLUDE_DIR})
target_include_directories(smkhe PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_include_directories(smkhe PUBLIC ${FLINT_INCLUDE_DIR})

include(FetchContent)

FetchContent_Declare(
        flint_src
        URL http://flintlib.org/flint-2.8.4.tar.gz
        DOWNLOAD_NO_EXTRACT true
)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)

FetchContent_MakeAvailable(googletest flint_src)

FILE(ARCHIVE_EXTRACT INPUT ${flint_src_SOURCE_DIR}/flint-2.8.4.tar.gz)

target_include_directories(smkhe PUBLIC ${CMAKE_SOURCE_DIR}/build/flint-2.8.4)

enable_testing()

add_executable(tests ${TEST_FILES})
target_link_libraries(tests gtest_main smkhe)

include(GoogleTest)
gtest_discover_tests(tests)
