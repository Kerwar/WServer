find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
# Generic test that uses conan libs
add_library(UriLib
    src/uri.cpp
    src/percent_encoded_character_decoder.cpp
    src/character_set.cpp
    src/normalize_case_insensitive_string.cpp
    )

target_link_libraries(
  UriLib 
  PUBLIC project_options project_warnings
  PRIVATE CLI11::CLI11 fmt::fmt spdlog::spdlog)

target_include_directories(UriLib PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
target_include_directories(UriLib PRIVATE headers)

add_subdirectory(test)
