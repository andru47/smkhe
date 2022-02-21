cmake_minimum_required(VERSION 3.10)
project(smkhe)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES ../src/encoder.cpp ../src/plaintext.cpp ../src/parameters.cpp ../src/public_key.cpp ../src/secret_key.cpp
        ../src/encryptor.cpp ../src/math/ntt_transformer.cpp ../src/util.cpp ../src/keygen.cpp ../src/rand_util.cpp ../src/ciphertext.cpp
        ../src/math/rns_transformer.cpp ../src/evaluation_key.cpp ../src/evaluator.cpp ../src/math/rns_converter.cpp
        ../build/lib/BigInt.cpp ../src/serializer_util.cpp
        ../src/mk_keygen.cpp ../src/mk_evaluator.cpp ../src/mk_public_key.cpp ../src/mk_ciphertext.cpp ../src/mk_evaluation_key.cpp ../src/mk_decryptor.cpp src/partial_ciphertext.cpp)
set(TEST_FILES ../test/encoder.cpp ../test/math/ntt_transformer.cpp ../test/encryptor.cpp ../test/evaluator.cpp ../test/serializer.cpp)

FILE(DOWNLOAD https://github.com/faheel/BigInt/releases/download/v0.5.0-dev/BigInt.hpp ../build/lib/BigInt.cpp)

include(FetchContent)

FetchContent_Declare(
        protobuf
        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
        GIT_TAG        v3.19.4
        SOURCE_SUBDIR  cmake
)

set(protobuf_BUILD_TESTS OFF)
FetchContent_MakeAvailable(protobuf)

add_library(protobuf_local STATIC IMPORTED)
set_target_properties(protobuf_local PROPERTIES IMPORTED_LOCATION /Users/andru/Desktop/libprotobuf.a)

add_library(smkhe STATIC ${SOURCE_FILES} ../build/messages.pb.cc ../build/messages.pb.h)

target_link_libraries(smkhe PUBLIC protobuf_local)
target_include_directories(smkhe PUBLIC ../include ../lib ${protobuf_SOURCE_DIR}/src)
