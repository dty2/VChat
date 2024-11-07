project(VChat-Server VERSION 1.0 LANGUAGES CXX)

find_package(glog REQUIRED PATHS ${GLOG_LIB})

# contain all sources
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES)
list(APPEND SOURCES
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/server.cpp
  ${SRC_DIR}/connection.cpp
  ${SRC_DIR}/service.cpp
  ${SRC_DIR}/store.cpp
)

add_executable(server ${SOURCES})

target_include_directories(server PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/net"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/data"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/service"
  "${BOOST_LIB}/include"
  "${SQLITE_LIB}/include"
  "${PROTOBUF_LIB}/include"
  "${HIREDIS_LIB}/include"
  "${REDIS_LIB}/include"
)

target_link_libraries(server
  "proto"
  "${HIREDIS_LIB}/lib/libhiredis.a"
  "${REDIS_LIB}/lib/libredis++.a"
  "${BOOST_LIB}/lib/libboost_system.a"
  "${BOOST_LIB}/lib/libboost_thread.a"
  "${BOOST_LIB}/lib/libboost_serialization.a"
  "${HIREDIS_LIB}/lib/libhiredis.a"
  "protobuf::libprotobuf"
  "OpenSSL::SSL"
  "OpenSSL::Crypto"
  "pthread"
  "SQLiteCpp"
  "absl::check"
  "absl::flags"
  "absl::flags_parse"
  "glog::glog"
)

# -g for gdb
set_target_properties(client PROPERTIES COMPILE_FLAGS "-g")
