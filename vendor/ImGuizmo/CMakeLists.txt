file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/*/*.cpp ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/*/*/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor)

add_library(ImGuizmo ${IMGUI_FILES} ${SRC_FILES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20")
target_link_libraries(ImGuizmo ImGui)

 