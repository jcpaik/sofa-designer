find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(smake smake.cc)
target_link_libraries(smake Boost::program_options)
