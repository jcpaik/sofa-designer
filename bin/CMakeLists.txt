find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(sbranch sbranch.cc)
target_link_libraries(sbranch sofa jsoncpp Boost::program_options)
