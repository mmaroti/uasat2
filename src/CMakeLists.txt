add_subdirectory(solvers)

add_executable(uasat-test test.cpp)
target_include_directories(uasat-test PUBLIC ../include)
target_link_libraries(uasat-test PUBLIC uasat-solvers)

find_program(UASAT_EMSCRIPTEN_PROG emcc)
if(UASAT_EMSCRIPTEN_PROG)
    message("Emscripten is enabled")
    add_custom_command(OUTPUT uasat.js
        COMMAND ${UASAT_EMSCRIPTEN_PROG}
            -std=c++11
            -I${CMAKE_CURRENT_SOURCE_DIR}/../include
            -I${CMAKE_CURRENT_SOURCE_DIR}/solvers/minisat
            ${UASAT_EMSCRIPTEN_SRCS}
            -o uasat.js
        DEPENDS ${UASAT_EMSCRIPTEN_SRCS})
    add_custom_target(uasat-emscripten ALL DEPENDS uasat.js)
else()
    message("Emscripten is not found")
endif()
