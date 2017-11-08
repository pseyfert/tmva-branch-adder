if(EXISTS $ENV{ROOTSYS}/cmake/ROOTConfig.cmake)
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
else()
  list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
endif()
find_package(ROOT REQUIRED COMPONENTS TMVA XMLIO TreePlayer MathMore Foam)
include(${ROOT_USE_FILE})

include(GNUInstallDirs)

add_executable(tmva-branch-adder blacklist.cpp main.cpp reader_wrapper.cpp)
target_link_libraries(tmva-branch-adder TMVA XMLIO TreePlayer Tree RIO Core)

if(iwyu_path)
  set_property(TARGET tmva-branch-adder PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
endif()

add_library(BranchAdder SHARED reader_wrapper.cpp blacklist.cpp)
set_target_properties(BranchAdder PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(BranchAdder PROPERTIES PUBLIC_HEADER reader_wrapper.h)

install(TARGETS tmva-branch-adder DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS BranchAdder
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES blacklist.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
