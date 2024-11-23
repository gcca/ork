cmake_minimum_required(VERSION 3.30)
project(
  ork
  VERSION 0.1.0
  LANGUAGES CXX)

option(ORK_DISABLE_TESTS "Disable tests" OFF)

find_package(Boost REQUIRED COMPONENTS program_options system thread json)
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)

add_library(ork-certs OBJECT ork/certs.cpp ork/utils.cpp ork/services.cpp)
target_link_libraries(ork-certs PUBLIC Boost::system Boost::json OpenSSL::SSL)

add_library(
  ork-lib OBJECT ork/handling.cpp ork/utils.cpp ork/handlers/user/create.cpp
                 ork/handlers/user/list.cpp)
target_compile_options(
  ork-lib
  PUBLIC -Wall
         -Wextra
         -Werror
         -Wpedantic
         -Wshadow
         -Weverything
         -Wconversion
         -Wsign-conversion
         -Wnon-virtual-dtor
         -Wold-style-cast
         -Wfloat-equal
         -Wformat=2
         -Wnull-dereference
         -Wundef
         -Wuninitialized
         -Wcast-align
         -Wformat-security
         -Wstrict-overflow
         -Wswitch-enum
         -Wunused-variable
         -Wunused-parameter
         -Wpointer-arith
         -Wcast-align
         -Wno-variadic-macros
         -fexceptions
         -fsafe-buffer-usage-suggestions
         -Wno-c++98-compat
         -Wno-padded
         -Wno-covered-switch-default
         -Wno-unsafe-buffer-usage)
target_link_libraries(
  ork-lib
  PUBLIC ork-certs
         Boost::program_options
         Boost::system
         Boost::thread
         Boost::json
         OpenSSL::SSL
         OpenSSL::Crypto
         PostgreSQL::PostgreSQL)

add_library(Ork::Ork INTERFACE IMPORTED)
set_target_properties(
  Ork::Ork
  PROPERTIES INTERFACE_SOURCES $<TARGET_OBJECTS:ork-lib>
             INTERFACE_INCLUDE_DIRECTORIES
             $<TARGET_PROPERTY:ork-lib,INTERFACE_INCLUDE_DIRECTORIES>
             INTERFACE_COMPILE_OPTIONS
             $<TARGET_PROPERTY:ork-lib,INTERFACE_COMPILE_OPTIONS>
             INTERFACE_LINK_LIBRARIES
             $<TARGET_PROPERTY:ork-lib,INTERFACE_LINK_LIBRARIES>)

add_executable(ork ork.cc)
target_link_libraries(ork Ork::Ork)

add_executable(ork-migration ork-migration.cc)
target_link_libraries(ork-migration PostgreSQL::PostgreSQL)

add_executable(ork-repl ork-repl.cc)
target_link_libraries(ork-repl ork-certs)
