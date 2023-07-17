# Header-only libraries
add_library(tqdm INTERFACE)
target_include_directories(tqdm INTERFACE "tqdm/")

add_library(jsoncpp jsoncpp.cpp)
target_include_directories(jsoncpp PUBLIC "json/")
