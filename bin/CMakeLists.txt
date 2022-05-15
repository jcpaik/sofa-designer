find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(example example.cc)
target_link_libraries(example Boost::program_options)
