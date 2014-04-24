cmake_minimum_required(VERSION 2.6)

project(sonido)

# Debug build ?
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Debug build")
  set(DEBUG_BUILD 1)

else (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DEBUG_BUILD 0)

endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Set the directory where to find cmake "modules"
set(CMAKE_MODULE_PATH ${sonido_SOURCE_DIR}/cmake)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(Platform)

message(STATUS "Procesing Source Code")

find_package(OpenAL)
find_package(Alure)
if(NOT OPENAL_FOUND OR NOT ALURE_FOUND)
  message(ERROR "OpenAL or Alure not found! main executable will not be hable to play the beeper")
endif(NOT OPENAL_FOUND OR NOT ALURE_FOUND)

# Include dirs
set(sonido_INCLUDE_DIRS
  ${PROJECT_BINARY_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${ALURE_INCLUDE_DIR}
  )

add_executable( sound
  ./main.cpp
  ./Blip_Buffer.cpp
  )

include_directories( sound
  ${sonido_INCLUDE_DIRS}
  )

target_link_libraries( sound
  ${OPENAL_LIBRARY}
  ${ALURE_LIBRARY}
  )

add_executable( sine
  ./sine.cpp
  )

include_directories( sine
  ${sonido_INCLUDE_DIRS}
  )

target_link_libraries( sine
  ${OPENAL_LIBRARY}
  ${ALURE_LIBRARY}
  )

