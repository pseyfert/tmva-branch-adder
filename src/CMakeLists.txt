if(EXISTS $ENV{ROOTSYS}/cmake/ROOTConfig.cmake)
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
else()
  list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
endif()
find_package(ROOT REQUIRED COMPONENTS TMVA XMLIO TreePlayer MathMore Foam)
include(${ROOT_USE_FILE})

include(GNUInstallDirs)

add_executable(tmva-branch-adder blacklist.cpp main.cpp reader_wrapper.cpp)
target_link_libraries(tmva-branch-adder PUBLIC TMVA XMLIO TreePlayer Tree RIO Core)

if(iwyu_path)
  set_property(TARGET tmva-branch-adder PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()

add_library(BranchAdder SHARED reader_wrapper.cpp blacklist.cpp)
target_include_directories(BranchAdder INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> 
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
set_target_properties(BranchAdder PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(BranchAdder PROPERTIES PUBLIC_HEADER reader_wrapper.h)

# inspired by https://github.com/tamaskenez/range-v3/commit/875065c5b5cc90bfcce1f9274c333484b4dada68
# add a "library" that isn't any shared library file, and put all dependencies which a
# BranchAdder library user would face in it.
add_library(tmva-branch-adder-dev INTERFACE)
# already set by ROOT_USE_FILE, just not as INTERFACE to propagate further (is PUBLIC better?)
target_include_directories(tmva-branch-adder-dev INTERFACE ${ROOT_INCLUDE_DIRS})
target_link_libraries(tmva-branch-adder-dev INTERFACE BranchAdder)
# use find_library such that a user gets the -L flag right
find_library(neededlibs NAMES TMVA XMLIO TreePlayer Tree RIO Core)
target_link_libraries(tmva-branch-adder-dev INTERFACE ${neededlibs})

install(TARGETS tmva-branch-adder
        EXPORT tmva-branch-adderTargets
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS BranchAdder
        EXPORT tmva-branch-adderTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS tmva-branch-adder-dev
        EXPORT tmva-branch-adderTargets
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES blacklist.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
