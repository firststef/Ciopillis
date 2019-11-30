cmake_minimum_required(VERSION 3.15)
project(Ciopillis)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-DSHARED=ON -DSTATIC=ON)
add_subdirectory(raylib)

add_subdirectory(CardGame)
add_subdirectory(ECSlib)
add_subdirectory(GameArena)
add_subdirectory(GameServer)