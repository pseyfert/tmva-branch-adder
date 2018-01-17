cmake_minimum_required(VERSION 3.3)
project(tmva-branch-adder VERSION 0.6)

# https://cmake.org/pipermail/cmake/2015-October/061886.html
include(CheckCXXCompilerFlag)
macro(AddCXXFlagIfSupported flag test)
  CHECK_CXX_COMPILER_FLAG(${flag} ${test})
  if( ${${test}} )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
  endif()
endmacro()

AddCXXFlagIfSupported(-Wall COMPILER_SUPPORTS_Wall)
AddCXXFlagIfSupported(-Wextra COMPILER_SUPPORTS_Wextra)
AddCXXFlagIfSupported(-Wshadow COMPILER_SUPPORTS_Wshadow)
AddCXXFlagIfSupported(-Wpedantic COMPILER_SUPPORTS_Wpedantic)

## NB: colorization is a bit of a mess with make vs. ninja vs -v
AddCXXFlagIfSupported(-fcolor-diagnostics COMPILER_SUPPORTS_fcolor-diagnostics) # e.g. clang 3.9
AddCXXFlagIfSupported(--fdiagnostics-color=always COMPILER_SUPPORTS_fdiagnostics-color-always) # e.g. gcc

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(NOT iwyu_path)
  message(STATUS "Could not find the program include-what-you-use")
endif()

add_subdirectory(src)


