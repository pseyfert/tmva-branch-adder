cmake_minimum_required(VERSION 3.3)
project(tmva-branch-adder VERSION 0.6 LANGUAGES CXX)

#### cmake features

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)


#### general compilation

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


#### dependencies

if(EXISTS $ENV{ROOTSYS}/cmake/ROOTConfig.cmake)
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
else()
  list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
endif()
find_package(ROOT REQUIRED COMPONENTS TMVA XMLIO TreePlayer MathMore Foam)
include(${ROOT_USE_FILE})


#### build exe

add_executable(tmva-branch-adder src/blacklist.cpp src/main.cpp src/reader_wrapper.cpp)
target_link_libraries(tmva-branch-adder TMVA XMLIO TreePlayer Tree RIO Core)
target_include_directories(tmva-branch-adder
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

if(iwyu_path)
  set_property(TARGET tmva-branch-adder PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()


#### build lib

add_library(BranchAdder SHARED src/reader_wrapper.cpp src/blacklist.cpp)
set_target_properties(BranchAdder PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(BranchAdder PROPERTIES PUBLIC_HEADER src/reader_wrapper.h)
# semi redundant due to ROOT_USE_FILE
target_include_directories(BranchAdder
  SYSTEM PUBLIC
  ${ROOT_INCLUDE_DIRS}
  )
target_include_directories(BranchAdder
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(BranchAdder
  INTERFACE
  TMVA XMLIO TreePlayer Tree RIO Core)


#### install exe
install(TARGETS tmva-branch-adder DESTINATION ${CMAKE_INSTALL_BINDIR})

#### install lib
install(TARGETS BranchAdder
        EXPORT tmva-branch-adder-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#### install header
install(FILES src/blacklist.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#### configure cmake modules
# configure tmva-branch-adder-config.cmake
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/tmva-branch-adder-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/tmva-branch-adder-config.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# configure tmva-branch-adder-targets.cmake
install(EXPORT tmva-branch-adder-targets
  FILE tmva-branch-adder-targets.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
  )

# configure tmva-branch-adder-config-version.cmake
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/tmva-branch-adder-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY ExactVersion
  )


#### install cmake modules
# install tmva-branch-adder-config-version.cmake
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/tmva-branch-adder-config-version.cmake
  DESTINATION ${INSTALL_CONFIGDIR})
# install tmva-branch-adder-config.cmake
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/tmva-branch-adder-config.cmake
  DESTINATION ${INSTALL_CONFIGDIR})
# install tmva-branch-adder-targets.cmake already done at configuration!
