cmake_minimum_required (VERSION 3.4.3)
project (ClangdIndexer)

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
link_directories(/home/xxx/llvm/llvm-8.0.0.src/build.clangd/lib)

llvm_map_components_to_libnames(LLVM_LIBS all)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(/home/xxx/local/libxml2/include/libxml2)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/clangd_include)
add_definitions(${LLVM_DEFINITIONS})
message("llvm-def:${LLVM_DEFINITIONS}")

include_directories(${LIBXML2_INCLUDE_DIRS})
add_executable(ClangdIndexer IndexerMain.cpp)

target_link_libraries(ClangdIndexer 
	clangDaemon
	clangAST
	clangBasic
	clangFrontend
	clangIndex
	clangLex
	clangTooling
	z rt dl tinfo pthread m xml2
)

