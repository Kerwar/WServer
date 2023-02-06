cmake_minimum_required(VERSION 3.15...3.25)

project(CmakeConfigPackageTests LANGUAGES CXX)
find_package(Catch2 CONFIG REQUIRED)
include(Catch)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(myproject CONFIG REQUIRED) # for intro, project_options, ...

  if(NOT TARGET myproject::project_options)
    message(FATAL_ERROR "Requiered config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

function(add_my_test test_to_add)
add_executable(${test_to_add} ${test_to_add}.cpp)
target_link_libraries(${test_to_add} PUBLIC Catch2::Catch2 internet_message)
#target_link_libraries(${test_to_add} PRIVATE myproject::project_warnings myproject::project_options catch_main)
target_link_libraries(${test_to_add} PRIVATE catch_main)

catch_discover_tests(${test_to_add}
  TEST_PREFIX
  "${test_to_add}."
    )
endfunction()

#add_library(catch_main OBJECT catch_main.cpp)
#target_link_libraries(catch_main PUBLIC Catch2::Catch2 )
#target_link_libraries(catch_main PRIVATE myproject::project_options)

list(APPEND test_sources
    test_internet_message
    )

foreach(file IN LISTS test_sources)
    add_my_test(${file})
endforeach()

