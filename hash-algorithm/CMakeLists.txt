file(GLOB_RECURSE srcs "*.c")
file(GLOB_RECURSE test_srcs "test_*.c")

list(REMOVE_ITEM srcs ${test_srcs})

add_library(hash STATIC ${srcs})
set_target_properties(hash PROPERTIES FOLDER "hash-algorithm")

foreach(test_file ${test_srcs})
    get_filename_component(target ${test_file} NAME_WE)
    add_executable(${target} ${test_file})
    target_link_libraries(${target} PRIVATE hash)    
    set_target_properties(${target} PROPERTIES FOLDER "hash-algorithm")
endforeach(test_file ${test_srcs})

unset(srcs)
unset(test_srcs)