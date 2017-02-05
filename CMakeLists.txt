cmake_minimum_required(VERSION 2.8)
project(RoboticHand)

find_package(Boost COMPONENTS system REQUIRED)
find_library(FACOM facom PATH /usr/local/lib/)

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
    src/websocket.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_executable(${PROJECT_NAME} ${SOURCES} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${FACOM})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
