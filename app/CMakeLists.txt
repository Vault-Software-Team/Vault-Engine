file(GLOB SRC_FILES src/*.cpp src/*/*.cpp src/*/*/*.cpp)
file(GLOB IMGUI_FILES {CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp {CMAKE_SOURCE_DIR}/vendor/imgui/*/*.cpp {CMAKE_SOURCE_DIR}/vendor/imgui/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_executable(app ${SRC_FILES} ${IMGUI_FILES} ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c)
target_link_libraries(app engine api glfw)

# if(DEFINED WINDOWS_BUILD)
#     message("Building for Windows")
#     target_link_libraries(app ${CMAKE_SOURCE_DIR}/windows/lib/libglfw3dll.a)
# else()
#     message("Building for Linux")
#     target_link_libraries(app ${CMAKE_SOURCE_DIR}/lib/libglfw.so)
# endif()
# target_link_libraries(app ${CMAKE_SOURCE_DIR}/lib/glad.o)