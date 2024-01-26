cmake_minimum_required(VERSION 3.27.7)
project(VAULT_ENGINE)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_subdirectory(api)
add_subdirectory(app)
add_subdirectory(engine)
add_subdirectory(hyperscript)
add_subdirectory(external/box2d)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/ImGuizmo)
if (NOT EMSCRIPTEN)
add_subdirectory(external/glfw)
else()
    include_directories(/usr/lib/emsdk/upstream/emscripten/cache/sysroot/include)
    set_target_properties(api
        PROPERTIES SUFFIX ".html"
        LINK_FLAGS " --bind -s WASM=0 -s MIN_WEBGL_VERSION=1 -s ABORT_ON_WASM_EXCEPTIONS=1 -g2 -s USE_GLFW=3")
    set_target_properties(app
        PROPERTIES SUFFIX ".html"
        LINK_FLAGS " --bind -s WASM=0 -s MIN_WEBGL_VERSION=1 -s ABORT_ON_WASM_EXCEPTIONS=1 -g2 -s USE_GLFW=3")
    set_target_properties(engine
        PROPERTIES SUFFIX ".html"
        LINK_FLAGS " --bind -s WASM=0 -s MIN_WEBGL_VERSION=1 -s ABORT_ON_WASM_EXCEPTIONS=1 -g2 -s USE_GLFW=3")
endif()

add_subdirectory(external/assimp)
add_subdirectory(external/yaml-cpp)

if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()

# file(GLOB SRC_FILES
#     "include/*.hpp"
#     "src/*.cpp"

#     "include/*/*.hpp"
#     "src/*/*.cpp"

#     "include/*/*/*.hpp"
#     "src/*/*/*.cpp"

#     "include/*/*/*/*.hpp"
#     "src/*/*/*/*.cpp"
# )

# include_directories(${CMAKE_SOURCE_DIR}/include)
# include_directories(${CMAKE_SOURCE_DIR}/vendor)

# add_executable(${PROJECT_NAME} ${SRC_FILES})
# # target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libGL.so)
# target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libglfw.so)
# target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/glad.o)