# set up catch2, which is header-only
add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE "catch2/")
# set up tqdm, which is header-only
add_library(tqdm INTERFACE)
target_include_directories(tqdm INTERFACE "tqdm/")
