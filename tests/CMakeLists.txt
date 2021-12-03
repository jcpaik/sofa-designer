# TODO: file(GLOB) is not a recommended way to locate all source codes
file(GLOB TEST_FILES "*.cc" "*.h")
add_executable(run_tests ${TEST_FILES})
target_link_libraries(run_tests PUBLIC catch2 sofa)
