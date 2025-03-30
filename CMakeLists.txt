cmake_minimum_required (VERSION 3.8)

project ("FlatHashMap")

add_subdirectory(3d-party/abseil-cpp)

add_executable (${PROJECT_NAME} "main.cpp" "FunnelFlatHashTable.hpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} absl::base absl::flat_hash_map)
