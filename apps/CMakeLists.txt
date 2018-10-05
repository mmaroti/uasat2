find_program(emsripten_prog em++)
if(NOT emsripten_prog)
    message("Emscripten not found")
else()
    message("Emscripten is enabled")
endif()

add_subdirectory(validate)
add_subdirectory(testing)
