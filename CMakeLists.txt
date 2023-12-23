cmake_minimum_required(VERSION 3.27.7)
project(VAULT_ENGINE)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_subdirectory(api)
add_subdirectory(app)
add_subdirectory(engine)
add_subdirectory(external/glfw)
add_subdirectory(external/assimp)

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