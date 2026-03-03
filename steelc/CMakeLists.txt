# steelc/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(steelc LANGUAGES CXX)

# Versioning
set(STEELC_VERSION_MAJOR 0)
set(STEELC_VERSION_MINOR 7)
set(STEELC_VERSION_PATCH 0)
set(STEELC_VERSION "${STEELC_VERSION_MAJOR}.${STEELC_VERSION_MINOR}.${STEELC_VERSION_PATCH}")

# C++ standard (should be at least C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)  # Treat warnings as errors
    add_compile_options(/permissive-)  # Stricter standards conformance
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Platform-specific libraries
add_library(platform_tools INTERFACE)

if(WIN32)
    find_library(DBGHELP_LIBRARY dbghelp)
    if(NOT DBGHELP_LIBRARY)
        message(FATAL_ERROR "dbghelp.lib not found!")
    endif()
    target_link_libraries(platform_tools INTERFACE ${DBGHELP_LIBRARY})
endif()

# LLVM configuration
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM directory: ${LLVM_DIR}")

llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
    irreader
    linker
    bitwriter
    passes
    analysis
    transformutils
    scalaropts
    ipo
    mc
    mcparser
    mcdisassembler
    targetparser
    codegen
    selectiondag
    target
    asmprinter
    native
    nativecodegen
)

add_library(llvm_libs INTERFACE)
target_include_directories(llvm_libs INTERFACE ${LLVM_INCLUDE_DIRS})
target_link_libraries(llvm_libs INTERFACE ${LLVM_LIBS})
target_compile_definitions(llvm_libs INTERFACE ${LLVM_DEFINITIONS})

# Steel core library
add_subdirectory("src")

# Main executable
add_executable(steelc
	"src/main.cpp"
)

target_link_libraries(steelc PRIVATE
	steelc_core
    platform_tools
)

# Executable properties
set_target_properties(steelc PROPERTIES
    VERSION ${STEELC_VERSION}
    SOVERSION ${STEELC_VERSION_MAJOR}
)

# Testing
option(ENABLE_TESTS "Build steelc tests" ON)
if(ENABLE_TESTS)
    add_subdirectory("tests")
    target_compile_definitions(steelc_core PUBLIC STEELC_ENABLE_TESTS)
endif()
