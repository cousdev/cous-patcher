cmake_minimum_required(VERSION 3.15)
project(cous-launcher C)
 
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
 
add_executable(cl
    src/main.c
    src/bundle.c
    src/plist.c
)

target_include_directories(cl 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)