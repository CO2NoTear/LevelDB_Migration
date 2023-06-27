# #
# @file CMakeLists.txt
# @brief
# db项目的根CMakeLists.txt
#
# @author niexw
# @email xiaowen.nie.cn@gmail.com
#
cmake_minimum_required(VERSION 3.2)
project(db)
message(STATUS "### Begin to configure project db ###")

# 检测cmake运行操作系统/CPU/编译器
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Building OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Building Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS ${PROJECT_SOURCE_DIR})

# 打开verbose选项
set(CMAKE_VERBOSE_MAKEFILE on)

# 编译模式，Release/Debug，-DCMAKE_BUILD_TYPE=Release|Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug) # Debug Release
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES "debug")
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Building mode: ${CMAKE_BUILD_TYPE}")

# c/c++按照11标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "C/C++ standard: ${CMAKE_CXX_STANDARD}")

# 设置编译结果输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin")

# gcc/g++ options
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    if(CMAKE_C_COMPILER_VERSION VERSION_LESS "6.3")
        message(FATAL_ERROR "The least version of gcc is 6.3")
    endif()

    set(CMAKE_C_FLAGS "-Wall -Werror -Wno-attributes -include leveldb/config.h")
    set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os")
    set(CMAKE_C_RELWITHDEBINFO_RELEASE "-O2")
    set(CMAKE_CXX_FLAGS "-Wall -Weffc++ -Werror -Wno-attributes -include leveldb/config.h -ftemplate-backtrace-limit=0 -fno-exceptions -fno-rtti")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")
    set(CMAKE_CXX_RELWITHDEBINFO_RELEASE "-O2")

    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Bsymbolic -Bsymbolic-functions -Wl,--no-undefined -fsanitize=address")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} -Bsymbolic -Bsymbolic-functions -Wl,--no-undefined -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -Bsymbolic -Bsymbolic-functions -Wl,--no-undefined -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s -Bsymbolic -Bsymbolic-functions -Wl,--no-undefined -fsanitize=address")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} -s -Bsymbolic -Bsymbolic-functions -Wl,--no-undefined -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s -Bsymbolic -Bsymbolic-functions -Wl,--no-undefined -fsanitize=address")
endif()

# 操作系统
set(Linux "Linux")

# Ensure we have pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-pthread")

if(HAVE_CLANG_THREAD_SAFETY)
    target_compile_options(leveldb
        PUBLIC
        -Werror -Wthread-safety)
endif(HAVE_CLANG_THREAD_SAFETY)

# cxx_feature_check(PTHREAD_AFFINITY)

# 检查数据类型
include(CheckTypeSize)
check_type_size(long SIZEOF_LONG)
check_type_size(wchar_t SIZEOF_WCHAR_T)

# 生成config.h文件
# configure_file(
# ${CMAKE_SOURCE_DIR}/config.h.in
# ${CMAKE_SOURCE_DIR}/include/leveldb/config.h
# NEWLINE_STYLE UNIX
# )

# 指定编译子目录
# add_subdirectory(src)
# add_subdirectory(tests)
find_package(Catch2 REQUIRED)
set(LEVELDB_PUBLIC_INCLUDE_DIR "include/leveldb")
set(LEVELDB_PORT_CONFIG_DIR "include/port")
configure_file(
    "port/port_config.h.in"
    "${PROJECT_BINARY_DIR}/${LEVELDB_PORT_CONFIG_DIR}/port_config.h"
)

# include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/db)
include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "."
)

add_library(DB "")
target_sources(DB
    PRIVATE

    # "${PROJECT_BINARY_DIR}/${LEVELDB_PORT_CONFIG_DIR}/port_config.h"
    "db/builder.cc"
    "db/builder.h"
    "db/c.cc"
    "db/db_impl.cc"
    "db/db_impl.h"
    "db/db_iter.cc"
    "db/db_iter.h"
    "db/dbformat.cc"
    "db/dbformat.h"
    "db/dumpfile.cc"
    "db/filename.cc"
    "db/filename.h"
    "db/log_format.h"
    "db/log_reader.cc"
    "db/log_reader.h"
    "db/log_writer.cc"
    "db/log_writer.h"
    "db/memtable.cc"
    "db/memtable.h"
    "db/repair.cc"
    "db/skiplist.h"
    "db/snapshot.h"
    "db/table_cache.cc"
    "db/table_cache.h"
    "db/version_edit.cc"
    "db/version_edit.h"
    "db/version_set.cc"
    "db/version_set.h"
    "db/write_batch_internal.h"
    "db/write_batch.cc"
    "port/port_stdcxx.h"
    "port/port.h"
    "port/thread_annotations.h"
    "table/block_builder.cc"
    "table/block_builder.h"
    "table/block.cc"
    "table/block.h"
    "table/filter_block.cc"
    "table/filter_block.h"
    "table/format.cc"
    "table/format.h"
    "table/iterator_wrapper.h"
    "table/iterator.cc"
    "table/merger.cc"
    "table/merger.h"
    "table/table_builder.cc"
    "table/table.cc"
    "table/two_level_iterator.cc"
    "table/two_level_iterator.h"
    "util/arena.cc"
    "util/arena.h"
    "util/bloom.cc"
    "util/cache.cc"
    "util/coding.cc"
    "util/coding.h"
    "util/comparator.cc"
    "util/crc32c.cc"
    "util/crc32c.h"
    "util/env.cc"
    "util/filter_policy.cc"
    "util/hash.cc"
    "util/hash.h"
    "util/logging.cc"
    "util/logging.h"
    "util/mutexlock.h"
    "util/no_destructor.h"
    "util/env_posix_test_helper.h"
    "util/options.cc"
    "util/random.h"
    "util/status.cc"
    $<$<VERSION_GREATER:CMAKE_VERSION,3.2>:PUBLIC>
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/c.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/cache.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/comparator.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/db.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/dumpfile.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/env.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/export.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/filter_policy.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/iterator.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/options.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/slice.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/status.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/table_builder.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/table.h"
    "${LEVELDB_PUBLIC_INCLUDE_DIR}/write_batch.h"
    "util/env_posix.cc"
    "util/posix_logger.h"
)

# target_include_directories(DB
# PUBLIC
# $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
# $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
# )
set(LEVELDB_PLATFORM_NAME LEVELDB_PLATFORM_POSIX)
target_compile_definitions(DB
    PRIVATE

    # Used by include/export.h when building shared libraries.
    LEVELDB_COMPILE_LIBRARY

    # Used by port/port.h.
    ${LEVELDB_PLATFORM_NAME}=1
)

enable_testing()
add_executable(TEST "")
target_sources(TEST
    PRIVATE

    "tests/memtableTest.cc"
    "tests/log_test.cc"
    "tests/skiplist_test.cc"
    "tests/arenaTest.cc"
    "tests/statusTest.cc"
    "tests/googletest_to_catchtest.cc")
target_link_libraries(TEST DB Catch2::Catch2WithMain)

add_test(NAME "tests" COMMAND "TEST")

message(STATUS "### Done ###")
