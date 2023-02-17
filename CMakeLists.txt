cmake_minimum_required(VERSION 3.5)

project(spf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(spf main.cpp)

install(TARGETS spf
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_link_libraries(spf -lresolv)
