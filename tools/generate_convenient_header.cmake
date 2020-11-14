set(INCLUDES "")

file(GLOB files "gpcl/*")
foreach(file ${files})
    get_filename_component(filename ${file} NAME)
    string(APPEND INCLUDES "#include <gpcl/${filename}>\n")
endforeach(file ${files})

configure_file("${CMAKE_CURRENT_LIST_DIR}/gpcl.hpp.in" gpcl.hpp)
