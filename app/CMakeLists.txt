file(GLOB SRC_FILES src/*.cpp src/*/*.cpp src/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_executable(app ${SRC_FILES} ${CMAKE_SOURCE_DIR}/engine/src/SceneSerialization.cpp ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c /usr/lib/emsdk/upstream/emscripten/system/include/emscripten.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20")
target_link_libraries(app engine api glfw assimp ImGui)

 