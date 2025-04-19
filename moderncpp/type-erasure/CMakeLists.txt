# Minimum CMake version required (FetchContent needs >= 3.11, gtest_discover_tests >= 3.10)
cmake_minimum_required(VERSION 3.14)

# Define the project name and language
project(myproject LANGUAGES CXX) # You can rename 'myproject' if you like

# Set the C++ standard (matches -std=c++20 from Makefile)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Optional: disable compiler-specific extensions for better portability

# --- Project Structure ---
# Define directories relative to the CMakeLists.txt location
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(BENCHMARK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/benchmark) # Define benchmark dir

# --- Compiler Flags (matches -Wall -g -O3 from Makefile) ---
# CMake handles optimization levels (-O3) via build types (e.g., Release).
# -g is typically added for Debug builds. -Wall is a good general warning.
# We add -Wall globally here. You can customize flags per build type if needed.
add_compile_options(-Wall)
# You could add other flags like -Wextra here if desired:
# add_compile_options(-Wall -Wextra)

# --- Include Directories ---
# The include directory is added per-target below using target_include_directories,
# which is generally preferred over the global include_directories().
# You could uncomment this if you have a strong reason for a global include path:
# include_directories(${INC_DIR})

# --- Source Files ---
# Find all .cpp files in the src directory (similar to wildcard)
file(GLOB SRC_FILES "${SRC_DIR}/*.cpp")

# --- Main Executable Target ---
# Define the executable named 'myprogram' (matches TARGET from Makefile)
add_executable(myprogram ${SRC_FILES})

# Ensure the executable can find headers in the include directory
target_include_directories(myprogram PRIVATE ${INC_DIR})

# --- Google Test Integration ---
# Enable testing support in CMake
enable_testing()

# Use FetchContent to download and prepare Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the Git repository and a tag (e.g., a release version)
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.16.0 # Using a recent tag, update if needed
)

# Ensure Google Mock is also available (often needed for matchers)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Make Google Test available (downloads and configures it)
FetchContent_MakeAvailable(googletest)

# --- Google Benchmark Integration ---
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.8.3 # Use a recent stable release tag, update if needed
)

# --- Add this line ---
# Tell Google Benchmark's build system NOT to build its own tests
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable building benchmark's tests" FORCE)
# -------
# Make Google Benchmark available (downloads and configures it)
FetchContent_MakeAvailable(googlebenchmark)


# --- Test Executable Target ---
# Find all .cpp files in the test directory
file(GLOB TEST_SRC_FILES "${TEST_DIR}/*.cpp")

# Only create the test target if test source files exist
if(TEST_SRC_FILES)
  # Define the test executable
  add_executable(runTests ${TEST_SRC_FILES})

  # Link the test executable against Google Test (GTest::gtest_main includes GTest::gtest)
  # This ALSO provides the necessary include directories for gtest and gmock
  target_link_libraries(runTests PRIVATE GTest::gtest_main)

  # Ensure the test executable can also find headers in the project's include directory
  target_include_directories(runTests PRIVATE ${INC_DIR})

  # Include the GoogleTest module to help discover tests
  include(GoogleTest)
  # Automatically add tests from the 'runTests' executable to CTest
  gtest_discover_tests(runTests)
else()
  message(STATUS "No source files found in ${TEST_DIR}. Skipping test target 'runTests'.")
endif()


# --- Benchmark Executable Target ---
# Find all .cpp files in the benchmark directory
file(GLOB BENCHMARK_SRC_FILES "${BENCHMARK_DIR}/*.cpp")

# Only create the benchmark target if benchmark source files exist
if(BENCHMARK_SRC_FILES)
  message(STATUS "Found benchmark sources, creating runBenchmarks target.")
  add_executable(runBenchmarks ${BENCHMARK_SRC_FILES})

  # Link benchmark executable against Google Benchmark libraries
  # benchmark::benchmark_main provides the main() function for benchmarks
  target_link_libraries(runBenchmarks PRIVATE benchmark::benchmark benchmark::benchmark_main)

  # Ensure benchmarks can find your project's headers
  target_include_directories(runBenchmarks PRIVATE ${INC_DIR})

  # If your benchmarks need to link against code from your main program,
  # you might need to create a library target for shared code or link
  # against the main executable target if appropriate (less common).
  # Example: target_link_libraries(runBenchmarks PRIVATE myprogram)

else()
  message(STATUS "No source files found in ${BENCHMARK_DIR}. Skipping target 'runBenchmarks'.")
endif()


# --- Build Directory & Clean ---
# CMake handles the build directory automatically (out-of-source builds are recommended).
# Build systems generated by CMake (like Makefiles or Ninja) provide a 'clean' target.
