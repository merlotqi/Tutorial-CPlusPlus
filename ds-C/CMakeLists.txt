file(GLOB_RECURSE srcs "*.c")
file(GLOB_RECURSE test_srcs "test_*.c")

list(REMOVE_ITEM srcs ${test_srcs})
message(STATUS "Found source files: ${srcs}")
message(STATUS "Found test source files: ${test_srcs}")

add_library(libds_c STATIC ${srcs})
set_target_properties(libds_c PROPERTIES FOLDER "ds-C")

foreach(test_file ${test_srcs})
    get_filename_component(target ${test_file} NAME_WE)
    add_executable(${target} ${test_file})
    target_link_libraries(${target} PRIVATE libds_c)    
    set_target_properties(${target} PROPERTIES FOLDER "ds-C")
endforeach(test_file ${test_srcs})
