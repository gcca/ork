cmake_minimum_required(VERSION 3.30)
project(ork VERSION 0.1.0 LANGUAGES CXX)

option(ORK_DISABLE_TESTS "Disable tests" OFF)

find_package(Boost REQUIRED COMPONENTS program_options system thread json)
find_package(OpenSSL REQUIRED)

add_executable(ork ork.cc certs.cpp)
target_compile_options(ork PRIVATE -Wall -Wextra -Werror -Wpedantic -Wshadow -Weverything -Wconversion -Wsign-conversion -Wnon-virtual-dtor -Wold-style-cast -Wfloat-equal -Wformat=2 -Wnull-dereference -Wundef -Wuninitialized -Wcast-align -Wformat-security -Wstrict-overflow -Wswitch-enum -Wunused-variable -Wunused-parameter -Wpointer-arith -Wcast-align -Wno-variadic-macros -fexceptions -fsafe-buffer-usage-suggestions -Wno-c++98-compat -Wno-padded -Wno-covered-switch-default -Wno-unsafe-buffer-usage)
target_link_libraries(ork PRIVATE Boost::program_options Boost::system Boost::thread Boost::json OpenSSL::SSL OpenSSL::Crypto)
