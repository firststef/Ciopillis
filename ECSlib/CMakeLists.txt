# File generated at : 15:17:50, Sat 30 Nov 
# Converted Project : ECSlib/ECSlib.vcxproj
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

##################### Variables ############################
# Change if you want modify path or other values           #
############################################################

# Project
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(DEPENDENCIES_DIR ${PROJECT_DIR}/dependencies)
set(PROJECT_NAME ECSlib)


# Outputs
set(OUTPUT_DEBUG /../ECSlib/ECSlibDir)
set(OUTPUT_RELEASE ${PROJECT_DIR}/bin)

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

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
endif()

# Messages
message("${PROJECT_NAME}: MAIN PROJECT: ${CMAKE_PROJECT_NAME}")
message("${PROJECT_NAME}: CURR PROJECT: ${CMAKE_CURRENT_SOURCE_DIR}")
message("${PROJECT_NAME}: CURR BIN DIR: ${CMAKE_CURRENT_BINARY_DIR}")

############### Files & Targets ############################
# Files of project and target to build                     #
############################################################

# Source Files
set(SRC_FILES
    ./External.cpp
    ./main.cpp
    ./PhyFunctions.cpp
    ./PhyFunctions2.cpp
    ./PhysicsComponent.cpp
    ./PhysicsSystem.cpp
)
source_group("Sources" FILES ${SRC_FILES})

# Header Files
set(HEADERS_FILES
    AnimationComponent.h
    AnimationEvent.h
    AnimationSystem.h
    Component.h
    Components.h
    DefferEvent.h
    DefferSystem.h
    DrawSystem.h
    ECS.h
    ECSlib.h
    Entity.h
    Event.h
    EventManager.h
    Events.h
    External.h
    GridAddRemoveEvent.h
    GridContainerChildComponent.h
    GridContainerComponent.h
    GridContainerSystem.h
    Helpers.h
    HitBoxComponent.h
    HitBoxEvent.h
    HitBoxSystem.h
    KeyboardEvent.h
    KeyboardInputComponent.h
    KeyboardInputSystem.h
    MouseEvent.h
    MouseInputSystem.h
    nvidia.h
    PhyFunctions.h
    PhyFunctions2.h
    PhysicsComponent.h
    PhysicsSystem.h
    Pool.h
    SpriteComponent.h
    System.h
    SystemControlEvent.h
    Systems.h
    TextureManager.h
    TransformComponent.h
)
source_group("Headers" FILES ${HEADERS_FILES})

# Add library to build.
add_library(${PROJECT_NAME} STATIC
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
    target_link_libraries(${PROJECT_NAME} raylib.lib GameServer.lib)
else(MSVC)
    target_link_libraries(${PROJECT_NAME} raylib GameServer )
endif(MSVC)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../raylib/src)
