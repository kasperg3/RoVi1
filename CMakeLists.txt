# Test CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# The name of the project
PROJECT(RoViMandatory2)
MESSAGE(${PROJECT_NAME} ":")

# Used to resolve absolute path names
SET(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Use c++11
SET(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Set the RobWork root (edit in .bashrc if necessary)
SET(RW_ROOT $ENV{RW_ROOT})

# Verbose mode prints more info when doing make
set(CMAKE_VERBOSE_MAKEFILE false)

# Set build type to release
SET(CMAKE_BUILD_TYPE Release)
MESSAGE("-- Build type: " ${CMAKE_BUILD_TYPE})

# Use RobWork
SET(RobWork_DIR ${RW_ROOT}/cmake) 
FIND_PACKAGE(RobWork REQUIRED)
find_package(OpenCV 3 REQUIRED)
INCLUDE_DIRECTORIES( ${ROBWORK_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${ROBWORK_LIBRARY_DIRS} )

# Compile options
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
  add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wcast-qual -Wcast-align)
endif()

# Add the source files to build
#SET(SrcFiles src/RoVi.cpp)
SET(SrcFiles src/seq2.cpp)

# The shared library to build:
ADD_EXECUTABLE(RoViMandatory2 ${SrcFiles})
TARGET_LINK_LIBRARIES(RoViMandatory2 ${ROBWORK_LIBRARIES} ${OpenCV_LIBS})

MESSAGE(${PROJECT_NAME} " done!")
