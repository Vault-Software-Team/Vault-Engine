file(GLOB SRC_FILES src/*.cpp src/*/*.cpp src/*/*/*.cpp)

if(WIN32)
    file(GLOB FREETYPE_LIBRARIES ${CMAKE_SOURCE_DIR}/windows/lib/libfreetype.dll.a)
else()
    find_package(Freetype REQUIRED)
endif()
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(api ${SRC_FILES} ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c
        ../include/Engine/Mono/Entity/Functions.hpp)
target_link_libraries(api glfw assimp ImGui ImGuizmo ${FREETYPE_LIBRARIES})
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -std=c++20")

 