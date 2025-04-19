# Minimum CMake version required
# NOTE: CMake 3.28+ provides the best native C++ Modules support.
# Using older versions will likely require significant custom commands
# or external tools for module dependency scanning.
cmake_minimum_required(VERSION 3.28) # <-- Updated for better module support

# Define the project name and language
project(myproject LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Enable C++ Module Support ---
# This tells CMake to scan C++20 sources for module dependencies ('import'/'export')
# and handle the necessary compilation order and flags automatically.
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
# Note: Ensure your compiler (Clang >= 16, GCC >= 12, MSVC >= VS2022 17.4+)
# has adequate C++20 module support.

# --- Project Structure ---
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(BENCHMARK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/benchmark)

# --- Compiler Flags ---
add_compile_options(-Wall)
# add_compile_options(-Wall -Wextra) # Optional

# --- Recursive File Discovery ---
# Find all .cppm files recursively within SRC_DIR
file(GLOB_RECURSE CORE_MODULE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${SRC_DIR}/*.cppm")

# Find all .cpp files recursively within SRC_DIR
file(GLOB_RECURSE ALL_CPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${SRC_DIR}/*.cpp")

# --- Identify main() source file ---
# IMPORTANT: Adjust this path if your main() is not in src/main.cpp
set(MAIN_CPP_FILE "src/main.cpp") # Path relative to CMAKE_CURRENT_SOURCE_DIR

# --- Separate main() from library sources ---
# Create a list of implementation files EXCLUDING the main file
list(FILTER ALL_CPP_FILES EXCLUDE REGEX "^${MAIN_CPP_FILE}$")
set(CORE_IMPL_FILES ${ALL_CPP_FILES}) # Rename the filtered list

# --- Core Library Target (Recommended for Modules & Shared Code) ---
# Put modules, implementations, and potentially headers into a library
add_library(core_library STATIC) # Or SHARED

# Use target_sources with FILE_SETs for module awareness
# BASE_DIRS is set to the top-level source dir because paths from GLOB_RECURSE are relative to it
target_sources(core_library
    # Regular Source Files (.cpp) - Excluding main.cpp
    # List these directly under the PRIVATE scope
    ${CORE_IMPL_FILES}

    # Module Interface Units (.cppm)
    # Use FILE_SET only for CXX_MODULES (and potentially HEADERS if needed)

    # Public interface sources (including module interfaces)
    PUBLIC
        FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} FILES
            ${CORE_MODULE_FILES} # Assuming CustomStringArray.cppm is in this list

    # Private implementation sources
    PRIVATE
        ${CORE_IMPL_FILES} # Assuming regular .cpp implementation files are here

    # Note: Headers (.h) are typically handled via include directories,
    # or potentially a FILE_SET HEADERS if you need finer control over header sets.
)

# Define include directories for the library and its consumers.
# PUBLIC: Consumers linking this library will inherit these.
# Adding SRC_DIR allows headers within src/ to be found (e.g., #include "subdir/header.h")
# Adding INC_DIR allows headers within include/ to be found (e.g., #include "public_header.h")
target_include_directories(core_library
    PUBLIC
        $<BUILD_INTERFACE:${SRC_DIR}>   # Headers within src/
        $<BUILD_INTERFACE:${INC_DIR}>   # Headers within include/
        $<INSTALL_INTERFACE:include> # For installation (adjust if needed)
)

# --- Main Executable Target ---
# Define the executable using ONLY the source file containing main()
add_executable(myprogram ${MAIN_CPP_FILE})

# Link the executable against the core library. This brings in all the
# compiled module and non-module code and necessary include paths.
target_link_libraries(myprogram PRIVATE core_library)

# --- Google Test Integration ---
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.16.0 # Or a newer tag
)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows
FetchContent_MakeAvailable(googletest)

# --- Google Benchmark Integration ---
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.8.3 # Or a newer tag
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable building benchmark's tests" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

# --- Test Executable Target ---
# Find test files recursively (if needed, otherwise keep simple GLOB)
file(GLOB_RECURSE TEST_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${TEST_DIR}/*.cpp")

if(TEST_SRC_FILES)
  add_executable(runTests ${TEST_SRC_FILES})

  # Link tests against the core library (to test its code) and Google Test
  # Include directories are handled by linking core_library and GTest::gtest_main
  target_link_libraries(runTests PRIVATE core_library GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(runTests)
else()
  message(STATUS "No source files found in ${TEST_DIR} or its subdirectories. Skipping test target 'runTests'.")
endif()

# --- Benchmark Executable Target ---
# Find benchmark files recursively (if needed)
file(GLOB_RECURSE BENCHMARK_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${BENCHMARK_DIR}/*.cpp")

if(BENCHMARK_SRC_FILES)
  message(STATUS "Found benchmark sources, creating runBenchmarks target.")
  add_executable(runBenchmarks ${BENCHMARK_SRC_FILES})

  # Link benchmarks against the core library and Google Benchmark
  # Include directories handled by linking core_library
  target_link_libraries(runBenchmarks PRIVATE core_library benchmark::benchmark benchmark::benchmark_main)

else()
  message(STATUS "No source files found in ${BENCHMARK_DIR} or its subdirectories. Skipping target 'runBenchmarks'.")
endif()


# --- Build Directory & Clean ---
# CMake handles the build directory automatically (out-of-source builds are recommended).
# Build systems generated by CMake (like Makefiles or Ninja) provide a 'clean' target.

# --- GLOB_RECURSE Caveat ---
# Note: Using file(GLOB...) means CMake won't automatically detect new/deleted files
# matching the pattern after the initial configuration. If you add/remove source files,
# you may need to manually re-run CMake (e.g., delete build dir or touch CMakeLists.txt).
# For maximum robustness, explicitly listing files is preferred, but GLOB_RECURSE
# is convenient for many files or deep hierarchies.
