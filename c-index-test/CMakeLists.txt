cmake_minimum_required (VERSION 3.4.3)
project (DemoIndexer)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

add_definitions(
-D__STDC_LIMIT_MACROS
-D__STDC_CONSTANT_MACROS
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -std=c++11 -fPIC")

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")

message("LLVM_INCLUDE_DIRS=${LLVM_INCLUDE_DIRS}")
message("LLVM_DEFINITIONS=${LLVM_DEFINITIONS}")
message("LLVM_LIB_DIRS=${LLVM_LIBRARY_DIR}")
message("Clang_LIB_DIRS=${CLANG_INSTALL_PREFIX}")
message("Clang_INCLUDES=${CLANG_INCLUDE_DIRS}")


link_directories(${LLVM_LIBRARY_DIR})
link_directories(/home/xxx/local/libxml2/lib/)
link_directories(/home/xxx/.jumbo/lib/)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(/home/xxx/local/libxml2/include/libxml2)
add_definitions(${LLVM_DEFINITIONS})
message("llvm-def:${LLVM_DEFINITIONS}")

include_directories(${LIBXML2_INCLUDE_DIRS})
set_property(
    SOURCE c-index-test.c
    PROPERTY COMPILE_FLAGS "-std=gnu89"
    )
add_executable(DemoIndexer core_main.cpp c-index-test.c)

target_link_libraries(DemoIndexer
    libclang
    clangAST
    clangBasic
    clangCodeGen
    clangFrontend
    clangIndex
    clangSerialization
z rt dl tinfo pthread m xml2
)
