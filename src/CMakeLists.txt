# #
# @file CMakeLists.txt
# @brief
# tests目录下cmake文件
#
# @author niexw
# @email xiaowen.nie.cn@gmail.com
#
include_directories(${CMAKE_SOURCE_DIR}/include)

set(LIB_STORE_IMPL store/arena.cc)
add_library(store STATIC ${LIB_STORE_IMPL})
