# Single &amp; Multi Key Homomorphic Encryption Library

This C++ library implements the Full-RNS CKKS Homomorphic Encryption scheme.
Apart from the basic use-case of CKKS, the library also implements the multi-key variant explained in this [paper](https://eprint.iacr.org/2019/524.pdf). The library doesn't implement Galois keys and can be used as a reference for future C++ multi-key CKKS libraries.
The library was created as part of final-year university project in order to compare with existing HE libraries while using [HE for contact tracing]().

:warning: It is an experimental library and should not be used in production.

# Build
The library depends on [Protobuf](https://github.com/protocolbuffers/protobuf) and [BigInt](https://github.com/faheel/BigInt). While building BigInt is included in CMake, Protobuf needs to be installed beforehand.

The command for building is:

1. ```cmake -B build .``` 
2. ```cmake --build build```

After this, the library will be created inside the `build` folder.

# Build for iOS/Android
The library can also be compiled for iOS/Android.

### iOS
Protobuf needs to be built firstly. Using their source code, we can generate a binary with CMake and Xcode. After this, the ``smkhe`` library can be built.
1. Generate the Xcode project with:
    
    ``cmake -S . -B build-ios -GXcode -DCMAKE_SYSTEM_NAME=iOS "-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64" -Dprotobuf_BUILD_TESTS=OFF``
2. Then, using the project created inside `build-ios`, the protobuf binary can be built.
3. Modify the `PROTOBUF_LIB` variable from line 6 of the [CMakeLists file](cmake/CMakeLists.txt) to point to the binary
4. Use CMake with the `CMakeLists` file from the `cmake` folder as in the first step to generate the iOS project of `smkhe`

### Android
1. Using the details from [here](https://developer.android.com/ndk/guides/cmake#usage), we can build the `Protobuf` binary using CMake
2. Modify the `PROTOBUF_LIB` variable from line 6 of the [CMakeLists file](cmake/CMakeLists.txt) to point to the binary
3. Similarly to 3, build the ``smkhe`` binary using CMake

# References

1. [Homomorphic encryption for arithmetic of approximate numbers](https://eprint.iacr.org/2016/421.pdf)
2. [Efficient multi-key homomorphic encryption with packed ciphertexts with application to oblivious neural network inference](https://eprint.iacr.org/2019/524.pdf)
3. [A full rns variant of approximate homomorphic encryption](https://eprint.iacr.org/2018/931.pdf)
4. [Faster homomorphic discrete fourier transforms and improved fhe bootstrapping](https://eprint.iacr.org/2018/1073.pdf)