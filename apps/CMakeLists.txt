# Iterates over each "*.cc" file and make corresponding target
# This is a sin
file(GLOB APP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cc")
foreach(testsourcefile ${APP_SOURCES})
  string(REPLACE ".cc" "" testname ${testsourcefile})
  add_executable(${testname} ${testsourcefile})
  target_link_libraries(${testname} PUBLIC sofa tqdm)
endforeach()
