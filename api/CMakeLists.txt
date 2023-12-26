file(GLOB SRC_FILES src/*.cpp src/*/*.cpp src/*/*/*.cpp)
file(GLOB IMGUI_FILES ${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp ${CMAKE_SOURCE_DIR}/vendor/imgui/*/*.cpp ${CMAKE_SOURCE_DIR}/vendor/imgui/*/*/*.cpp)

find_package(Freetype REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(api ${SRC_FILES} ${IMGUI_FILES} ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c /usr/lib/emsdk/upstream/emscripten/system/include/emscripten.h)
target_link_libraries(api glfw assimp ${FREETYPE_LIBRARIES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")