project(VChat-Client VERSION 1.0 LANGUAGES CXX)

find_package(X11 REQUIRED) # need by QHotkey
find_package(Qt5 REQUIRED COMPONENTS Widgets X11Extras)
find_package(QHotkey REQUIRED PATHS ${HOTKEY_LIB})

# Qt set
# auto handle .ui file
set(CMAKE_AUTOUIC ON)
# automatically enable the meta-object compiler (supports signals and slots)
set(CMAKE_AUTOMOC ON)
# automatically process qt resource files
set(CMAKE_AUTORCC ON)

# contain all sources
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(UI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ui")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# pay attention: all .ui, .h and .cpp files should be included here
set(SOURCES)
list(APPEND SOURCES
  "${INCLUDE_DIR}/data.hpp"
  "${INCLUDE_DIR}/store.h"
  "${INCLUDE_DIR}/net.h"
  "${INCLUDE_DIR}/chat.h"
  "${INCLUDE_DIR}/error.h"
  "${INCLUDE_DIR}/dashboard.h"
  "${INCLUDE_DIR}/personalinfo.h"
  "${UI_DIR}/error.ui"
  "${UI_DIR}/chat.ui"
  "${UI_DIR}/dashboard.ui"
  "${UI_DIR}/personalinfo.ui"
  "${SRC_DIR}/personalinfo.cpp"
  "${SRC_DIR}/dashboard.cpp"
  "${SRC_DIR}/chat.cpp"
  "${SRC_DIR}/error.cpp"
  "${SRC_DIR}/store.cpp"
  "${SRC_DIR}/net.cpp"
  "${SRC_DIR}/main.cpp"
)

add_executable(client ${SOURCES})

target_include_directories(client PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${BOOST_LIB}/include"
  "${SQLITE_LIB}/include"
  "${HOTKEY_LIB}/include"
  "${PROTOBUF_LIB}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/generate/sources/"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)

target_link_libraries(client
  proto
  X11
  Qt5::Widgets
  Qt5::X11Extras
  ${HOTKEY_LIB}/lib/libqhotkey.a
  ${BOOST_LIB}/lib/libboost_system.a
  ${BOOST_LIB}/lib/libboost_thread.a
  ${BOOST_LIB}/lib/libboost_serialization.a
  OpenSSL::SSL
  OpenSSL::Crypto
  absl::check
  absl::flags
  absl::flags_parse
  protobuf::libprotobuf
  pthread
  SQLiteCpp
)

# set target property
set_target_properties(client PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# -g for gdb
set_target_properties(client PROPERTIES COMPILE_FLAGS "-g")
