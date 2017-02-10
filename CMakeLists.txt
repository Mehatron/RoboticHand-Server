cmake_minimum_required(VERSION 2.8)
project(RoboticHand)

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER /usr/bin/armv6j-hardfloat-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/armv6j-hardfloat-linux-gnueabi-g++)

SET(CMAKE_FIND_ROOT_PATH /usr/armv6j-hardfloat-linux-gnueabi/)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Boost COMPONENTS system REQUIRED)
find_library(FACOM facom PATH /usr/armv6j-hardfloat-linux-gnueabi/lib/)
find_package(Threads REQUIRED)

if(NOT FACOM)
    message(FATAL_ERROR "ERROR: Can't find FaCom")
endif()

include_directories(include/)
include_directories(
    libs/
    ${Boost_INCLUDE_DIR}
)

set(SOURCES
    src/robotichand.cpp
    src/server.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_executable(${PROJECT_NAME} ${SOURCES} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${FACOM} Threads::Threads)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
