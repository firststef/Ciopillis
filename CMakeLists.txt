cmake_minimum_required(VERSION 3.12)
project(Ciopillis)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(BUILD_EXAMPLES CACHE BOOL OFF)
    set(BUILD_GAMES CACHE BOOL OFF)
else()
    set(BUILD_EXAMPLES OFF)
    set(BUILD_GAMES OFF)
endif()
add_compile_options(-DSHARED=ON -DSTATIC=ON)

add_definitions(-DCIOPILLIS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(ECSlib)
add_subdirectory(CardGame)
add_subdirectory(GameArena)
add_subdirectory(GameServer)
add_subdirectory(CiopillisServer)
add_subdirectory(CiopillisClient)

