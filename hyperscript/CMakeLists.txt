file(GLOB SRC_FILES src/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp src/*/*/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(hyperscript ${SRC_FILES} )
target_link_libraries(hyperscript yaml-cpp)
target_precompile_headers(hyperscript PRIVATE ${CMAKE_SOURCE_DIR}/vendor/exprtk.hpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20")

 