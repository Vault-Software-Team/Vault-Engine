file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp ${CMAKE_SOURCE_DIR}vendor/imgui/backends/*.cpp ${CMAKE_SOURCE_DIR}vendor/imgui/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(ImGui ${SRC_FILES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20")

 