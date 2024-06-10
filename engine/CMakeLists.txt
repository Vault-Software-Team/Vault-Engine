file(GLOB SRC_FILES src/*.cpp src/SceneSerialization.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp src/*/*/*/*/*.cpp)
file(GLOB IMGUI_FILES ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuiFileDialog/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(engine ${SRC_FILES} ${IMGUI_FILES} ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c )

if(WIN32)
    target_link_libraries(engine ${CMAKE_SOURCE_DIR}/windows/lib/libmono-2.0.dll.a ${CMAKE_SOURCE_DIR}/windows/lib/libSDL2_mixer.dll.a ${CMAKE_SOURCE_DIR}/windows/lib/libSDL2.dll.a ${CMAKE_SOURCE_DIR}/windows/lib/libmonosgen-2.0.dll.a api glfw yaml-cpp tobanteGaming::Box2D ImGui ImGuizmo)
else()
    link_directories(${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(engine ${CMAKE_SOURCE_DIR}/lib/libSDL2_mixer.so api glfw efsw yaml-cpp tobanteGaming::Box2D ImGui ImGuizmo)
endif()
 
# target_link_libraries(engine ${CMAKE_SOURCE_DIR}/windows/lib/libmono-2.0.dll.a ${CMAKE_SOURCE_DIR}/windows/lib/libmonosgen-2.0.dll.a api glfw SDL_mixer yaml-cpp tobanteGaming::Box2D ImGui ImGuizmo)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -std=c++20")

 