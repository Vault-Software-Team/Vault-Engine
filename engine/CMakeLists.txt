file(GLOB SRC_FILES src/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp src/*/*/*/*/*.cpp)
file(GLOB IMGUI_FILES ${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp ${CMAKE_SOURCE_DIR}/vendor/imgui/*/*.cpp ${CMAKE_SOURCE_DIR}/vendor/imgui/*/*/*.cpp)
file(GLOB IMGUI_FILES ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*/*/*.cpp)
file(GLOB IMGUIZMO_FILES ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/*/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(engine ${SRC_FILES} ${IMGUI_FILES} ${IMGUIZMO_FILES} ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c /usr/lib/emsdk/upstream/emscripten/system/include/emscripten.h)
target_link_libraries(engine api glfw yaml-cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20")