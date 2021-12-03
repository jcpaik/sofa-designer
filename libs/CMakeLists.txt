# set up lib/sofa, which depends on CGAL
find_package(CGAL REQUIRED COMPONENTS Core)
if(NOT CGAL_FOUND)
  message(FATAL_ERROR "This program requires the CGAL library, and cannot be compiled.")
endif()

# TODO: file(GLOB) is not a recommended way to locate all source codes
file(GLOB SOFA_FILES "sofa/*.cc" "sofa/*.h")
add_library(sofa STATIC ${SOFA_FILES})
target_include_directories(sofa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sofa PUBLIC CGAL::CGAL CGAL::CGAL_Core tqdm)
