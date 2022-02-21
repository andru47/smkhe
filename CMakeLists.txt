cmake_minimum_required(VERSION 3.10)
project(smkhe)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/encoder.cpp src/plaintext.cpp src/parameters.cpp src/public_key.cpp src/secret_key.cpp
                 src/encryptor.cpp src/math/ntt_transformer.cpp src/util.cpp src/keygen.cpp src/rand_util.cpp src/ciphertext.cpp
                 src/math/rns_transformer.cpp src/evaluation_key.cpp src/evaluator.cpp src/math/rns_converter.cpp
                 build/lib/BigInt.cpp src/serializer_util.cpp
                 src/mk_keygen.cpp src/mk_evaluator.cpp src/mk_public_key.cpp src/mk_ciphertext.cpp src/mk_evaluation_key.cpp src/mk_decryptor.cpp src/partial_ciphertext.cpp)
set(TEST_FILES test/encoder.cpp test/math/ntt_transformer.cpp test/encryptor.cpp test/evaluator.cpp test/serializer.cpp)

FILE(DOWNLOAD https://github.com/faheel/BigInt/releases/download/v0.5.0-dev/BigInt.hpp build/lib/BigInt.cpp)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS src/serialization/messages.proto)
add_library(smkhe SHARED ${SOURCE_FILES} ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(smkhe PRIVATE ${Protobuf_LIBRARIES})
target_include_directories(smkhe PUBLIC ./include ./lib ${Protobuf_INCLUDE_DIRS})

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
gtest_discover_tests(tests PROPERTIES DISCOVERY_TIMEOUT 600)
