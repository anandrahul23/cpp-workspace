# Replace /path/to/your/sdk with the actual path from xcrun

SDK_PATH=$(xcrun --show-sdk-path)

cmake -S . -B build -G Ninja \
 -DCMAKE_BUILD_TYPE=Debug \
 -DCMAKE_C_COMPILER=/usr/local/opt/llvm/bin/clang \
 -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ \
 -DCMAKE_OSX_SYSROOT="${SDK_PATH}" \
 -DCMAKE_CXX_FLAGS="-isysroot ${SDK_PATH}" \
 -DCMAKE_C_FLAGS="-isysroot ${SDK_PATH}" # Also add for C compiler just in case
