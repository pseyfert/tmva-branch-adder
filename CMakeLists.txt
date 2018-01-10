cmake_minimum_required(VERSION 3.3)
project(tmva-branch-adder VERSION 0.5)
# TODO: doesn't get exported
message(STATUS "building tmva-branch-adder version ${tmva-branch-adder_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow")
find_program(iwyu_path NAMES include-what-you-use iwyu)
if(NOT iwyu_path)
  message(STATUS "Could not find the program include-what-you-use")
endif()
add_subdirectory(src)

install(EXPORT tmva-branch-adderTargets FILE tmva-branch-adder-config.cmake
  DESTINATION share/cmake)
