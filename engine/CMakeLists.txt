file(GLOB SRC_FILES src/*.cpp src/*/*.cpp src/*/*/*.cpp)
file(GLOB IMGUI_FILES ${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp ${CMAKE_SOURCE_DIR}/vendor/imgui/*/*.cpp ${CMAKE_SOURCE_DIR}/vendor/imgui/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(engine ${SRC_FILES} ${IMGUI_FILES} ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c)
target_link_libraries(engine api glfw)