add_subdirectory(solvers)

add_executable(uasat-test test.cpp)
target_include_directories(uasat-test PUBLIC ../include)
target_link_libraries(uasat-test PUBLIC uasat-solvers)

find_program(UASAT_EMSCRIPTEN_PROG em++)
if(NOT UASAT_EMSCRIPTEN_PROG)
    message("Emscripten is not found")
else()
    message("Emscripten found")
    list(APPEND UASAT_EMSCRIPTEN_SRCS
        ${UASAT_EMSCRIPTEN_SOLVER_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
    add_custom_command(OUTPUT uasat.js
        COMMAND ${UASAT_EMSCRIPTEN_PROG}
            -std=c++11
            -O2
            -I${CMAKE_CURRENT_SOURCE_DIR}/../include
            -I${CMAKE_CURRENT_SOURCE_DIR}/solvers/minisat
            ${UASAT_EMSCRIPTEN_SRCS}
            -s "MODULARIZE=1"
            -o uasat.js
        DEPENDS ${UASAT_EMSCRIPTEN_SRCS})
    add_custom_target(uasat-emscripten ALL DEPENDS uasat.js)
endif()
