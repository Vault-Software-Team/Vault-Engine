file(GLOB SRC_FILES src/*.cpp src/*/*.cpp src/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

# if(BUILD_GAME)
#     add_compile_definitions(GAME_BUILD)
# endif()
add_executable(app ${SRC_FILES} ${CMAKE_SOURCE_DIR}/engine/src/SceneSerialization.cpp ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c )
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3 -std=c++20")

set(CMAKE_CXX_STANDARD 20)


if(WIN32)
    target_link_libraries(app ${CMAKE_SOURCE_DIR}/windows/lib/libmono-2.0.dll.a ${CMAKE_SOURCE_DIR}/windows/lib/libmonosgen-2.0.dll.a engine efsw api glfw assimp ImGui)
else()
    link_directories(${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(app mono-2.0 mono-native monosgen-2.0 engine api glfw assimp ImGui)
endif()
