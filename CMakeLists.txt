cmake_minimum_required(VERSION 2.8)
project(RoboticHand)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER /usr/bin/armv6j-hardfloat-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/armv6j-hardfloat-linux-gnueabi-g++)

set(CMAKE_FIND_ROOT_PATH /usr/armv6j-hardfloat-linux-gnueabi/)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(Boost_USE_STATIC_LIBS   ON)

find_package(Boost COMPONENTS system REQUIRED)
find_library(FACOM facom PATH /usr/armv6j-hardfloat-linux-gnueabi/lib/)
find_library(UNIPROT uniprot PATH /usr/armv6j-hardfloat-linux-gnueabi/lib/)
find_package(Threads REQUIRED)

if(NOT FACOM)
    message(FATAL_ERROR "ERROR: Can't find FaCom")
endif()
if(NOT UNIPROT)
    message(FATAL_ERROR "ERROR: Can't find UniProt")
endif()

include_directories(include/)
include_directories(
    libs/
    ${Boost_INCLUDE_DIR}
)

set(SOURCES
    src/robotichand.cpp
    src/socket.cpp
    src/socketserver.cpp
    src/server.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_executable(${PROJECT_NAME} ${SOURCES} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${FACOM} ${UNIPROT} Threads::Threads)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
