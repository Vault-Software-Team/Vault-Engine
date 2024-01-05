file(GLOB SRC_FILES src/*.cpp src/SceneSerialization.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp src/*/*/*/*/*.cpp)
file(GLOB IMGUI_FILES ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(engine ${SRC_FILES} ${IMGUI_FILES} ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c /usr/lib/emsdk/upstream/emscripten/system/include/emscripten.h)
target_link_libraries(engine api glfw yaml-cpp tobanteGaming::Box2D ImGui ImGuizmo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20")

 