add_executable(validate validate/validate.cpp)
target_include_directories(validate PUBLIC ../include)
target_link_libraries(validate PUBLIC uasat-solvers)

find_program(UASAT_EMSCRIPTEN_PROG em++)
if(NOT UASAT_EMSCRIPTEN_PROG)
    message("Emscripten is not found")
else()
    message("Emscripten found")
    list(APPEND UASAT_EMSCRIPTEN_SRCS
        ${uasat_emscripten_solvers_srcs}
        ${CMAKE_CURRENT_SOURCE_DIR}/validate/validate.cpp)
    add_custom_command(OUTPUT validate.js
        COMMAND ${UASAT_EMSCRIPTEN_PROG}
            -std=c++11
            -O2
            -I${CMAKE_CURRENT_SOURCE_DIR}/../include
            -I${CMAKE_CURRENT_SOURCE_DIR}/../src/solvers/minisat
            ${UASAT_EMSCRIPTEN_SRCS}
            -s "MODULARIZE=1"
            -o validate.js
        DEPENDS ${UASAT_EMSCRIPTEN_SRCS})
    add_custom_target(validate-js ALL DEPENDS validate.js)
endif()
