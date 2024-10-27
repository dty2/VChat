cmake_minimum_required(VERSION 3.5)

project(VChat VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# env
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "Release") # uncomment to release

# generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# libs
set(LIB "${CMAKE_SOURCE_DIR}/libs")
set(HOTKEY_LIB "${LIB}/QHotkey")
set(PROTOBUF_LIB "${LIB}/protobuf")
set(BOOST_LIB "${LIB}/boost_1_85_0")
set(HIREDIS_LIB "${LIB}/hiredis")
set(REDIS_LIB "${LIB}/redis")
set(SQLITE_LIB "${LIB}/SQLiteCpp")
set(GLOG_LIB "${LIB}/glog")

# set boost
add_definitions(-DBOOST_ASIO_SEPARATE_COMPILATION)
# set protobuf
# option(protobuf_MODULE_COMPATIBLE TRUE)

find_package(protobuf CONFIG REQUIRED PATHS ${PROTOBUF_LIB})
find_package(OpenSSL REQUIRED)
find_package(SQLiteCpp REQUIRED PATHS ${SQLITE_LIB})
find_package(Boost 1.85.0 REQUIRED
  COMPONENTS system serialization
  PATHS "${BOOST_LIB}"
)

# persional protocol use profobuf
set(GENERATE_DIR "${CMAKE_SOURCE_DIR}/protobuf/generate")
include_directories("${GENERATE_DIR}")
add_library(proto
  "${GENERATE_DIR}/vchat.pb.cc"
  "${GENERATE_DIR}/vchat.pb.h"
)

target_link_libraries(proto
  absl::check
  absl::flags
  absl::flags_parse
  protobuf::libprotobuf
)

add_subdirectory(VChat-Client)
add_subdirectory(VChat-Server)
