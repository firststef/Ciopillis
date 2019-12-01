cmake_minimum_required(VERSION 3.15)
project(Ciopillis)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_EXAMPLES CACHE BOOL OFF)
set(BUILD_GAMES CACHE BOOL OFF)
add_compile_options(-DSHARED=ON -DSTATIC=ON)
add_subdirectory(raylib)

add_subdirectory(ECSlib)
add_subdirectory(CardGame)
add_subdirectory(GameArena)
add_subdirectory(GameServer)
