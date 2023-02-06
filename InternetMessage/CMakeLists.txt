set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
# Generic test that uses conan libs
add_library(internet_message
    src/internet_message.cpp
    )

target_link_libraries(
  internet_message 
  PUBLIC project_options project_warnings)

target_include_directories(internet_message PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
target_include_directories(internet_message PRIVATE headers)

add_subdirectory(test)
