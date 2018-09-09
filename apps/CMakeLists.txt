add_executable(validate validate/validate.cpp)
target_include_directories(validate PUBLIC ../include)
target_link_libraries(validate PUBLIC uasat)

find_program(emsripten_prog em++)
if(NOT emsripten_prog)
    message("Emscripten is not found")
else()
    message("Emscripten found")
    list(APPEND validate_emscripten_srcs
        ${uasat_emscripten_srcs}
        ${CMAKE_CURRENT_SOURCE_DIR}/validate/validate.cpp)
    add_custom_command(OUTPUT validate.js
        COMMAND ${emsripten_prog}
            -std=c++11
            -O2
            -I${CMAKE_CURRENT_SOURCE_DIR}/../include
            -I${CMAKE_CURRENT_SOURCE_DIR}/../src/solvers
            -I${CMAKE_CURRENT_SOURCE_DIR}/../src/solvers/minisat
            ${validate_emscripten_srcs}
            -s "MODULARIZE=1"
            -o validate.js
        DEPENDS ${validate_emscripten_srcs})
    add_custom_target(validate-js ALL DEPENDS validate.js)
endif()
