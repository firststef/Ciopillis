# File generated at : 15:17:50, Sat 30 Nov
# Converted Project : CardGame/CardGame.vcxproj
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

##################### Variables ############################
# Change if you want modify path or other values           #
############################################################

# Project
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(DEPENDENCIES_DIR ${PROJECT_DIR}/dependencies)
set(PROJECT_NAME CardGame)


# Outputs
set(OUTPUT_DEBUG Debug)
set(OUTPUT_RELEASE Release)

################# CMake Project ############################
# The main options of project                              #
############################################################

project(${PROJECT_NAME} CXX)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

############## Artefacts Output ############################
# Defines outputs , depending BUILD TYPE                   #
############################################################

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")

# Messages
message("${PROJECT_NAME}: MAIN PROJECT: ${CMAKE_PROJECT_NAME}")
message("${PROJECT_NAME}: CURR PROJECT: ${CMAKE_CURRENT_SOURCE_DIR}")
message("${PROJECT_NAME}: CURR BIN DIR: ${CMAKE_CURRENT_BINARY_DIR}")

############### Files & Targets ############################
# Files of project and target to build                     #
############################################################

# Source Files
set(SRC_FILES
    ./main.cpp
)
source_group("Sources" FILES ${SRC_FILES})

# Header Files
set(HEADERS_FILES
    CardComponent.h
    CardGameComponents.h
    Constants.h
    EnemyEvent.h
    EnemySystem.h
    CardGameEvents.h
    EventSystem.h
    CardGameSystems.h
)
source_group("Headers" FILES ${HEADERS_FILES})

if(MSVC)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
set(RAYLIB_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src/Debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
set(RAYLIB_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src/Release)
endif()
include_directories(${RAYLIB_LIB_DIR})
endif()

# Add executable to build.
add_executable(${PROJECT_NAME}
   ${SRC_FILES} ${HEADERS_FILES}
)

######################### Flags ############################
# Defines Flags for Windows and Linux                      #
############################################################

if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   endif()
endif(NOT MSVC)

# Preprocessor definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
   -DGRAPHICS_API_OPENGL_33
   -DPLATFORM_DESKTOP
   -D_CRT_SECURE_NO_WARNINGS
   -D_CRT_NONSTDC_NO_DEPRECATE
    )
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /MD /Od /EHsc)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
   -DGRAPHICS_API_OPENGL_33
   -DPLATFORM_DESKTOP
   -D_CRT_SECURE_NO_WARNINGS
    )
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /GL /Oi /Gy /EHsc)
    endif()
endif()

########### Link & Dependencies ############################
# Add project dependencies and Link to project             #
############################################################

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src/Debug/raylib_static.lib GameServer.lib ECSlib.lib )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src/Release/raylib_static.lib GameServer.lib ECSlib.lib )
    endif()
else(MSVC)
    target_link_libraries(${PROJECT_NAME} raylib GameServer ECSlib)
endif(MSVC)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src/ ${CMAKE_CURRENT_SOURCE_DIR}/../ECSlib ${CMAKE_CURRENT_SOURCE_DIR}/../GameServer)
