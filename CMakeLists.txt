cmake_minimum_required(VERSION 3.30)
project(
  ork
  VERSION 0.1.0
  LANGUAGES CXX)

option(ORK_DISABLE_TESTS "Disable tests" OFF)

find_package(Boost REQUIRED COMPONENTS program_options system thread json url)
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)

add_library(
  ork-lib OBJECT
  ork/handlers/doc/swagger.cpp
  ork/handlers/doc/redoc.cpp
  ork/handlers/doc/yaml.cpp
  ork/handlers/gathering/book.cpp
  ork/handlers/gathering/summary.cpp
  ork/handlers/model/create.cpp
  ork/handlers/model/list.cpp
  ork/handlers/recipe/cook.cpp
  ork/handlers/recipe/cooks.cpp
  ork/handlers/recipe/utils.cpp
  ork/handlers/user/create.cpp
  ork/handlers/user/list.cpp
  ork/handlers/user/models.cpp
  ork/services/completion.cpp
  ork/services/completion/certs.cpp
  ork/services/persistence/pq.cpp
  ork/conf.cpp
  ork/errors.cpp
  ork/handling.cpp
  ork/server.cpp)
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
target_include_directories(ork-lib PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(
  ork-lib
  PUBLIC Boost::program_options
         Boost::system
         Boost::thread
         Boost::json
         Boost::url
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

add_executable(ork-migration ork-migration.cc ork/migration.cpp)
target_link_libraries(ork-migration PostgreSQL::PostgreSQL)

add_executable(ork-repl ork-repl.cc)
target_link_libraries(ork-repl Ork::Ork)

if(NOT ORK_DISABLE_TESTS)
  find_package(GTest REQUIRED)
  include(GoogleTest)
  enable_testing()

  add_executable(ork-test ork/migration.cpp ork/testing.cpp
                          ork/handlers/model/create-test.cc
                          ork/handlers/model/list-test.cc
                          ork/handlers/user/create-test.cc
                          ork/handlers/user/list-test.cc)
  target_link_libraries(ork-test Ork::Ork GTest::GTest GTest::Main)
  target_compile_options(ork-test PUBLIC -Wno-c++98-compat-pedantic
                                         -Wno-global-constructors
                                         -Wno-weak-vtables)

  gtest_discover_tests(ork-test)
endif()
