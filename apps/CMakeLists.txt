add_executable(validate validate/validate.cpp)
target_include_directories(validate PUBLIC ../include)
target_link_libraries(validate PUBLIC uasat)

find_program(emsripten_prog em++)
if(NOT emsripten_prog)
    message("Emscripten is not found")
else()
    message("Emscripten found")
endif()

if(emsripten_prog)
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

add_executable(testing testing/testing.cpp)
target_include_directories(testing PUBLIC ../include)
target_link_libraries(testing PUBLIC uasat)

if(emsripten_prog)
    list(APPEND testing_emscripten_srcs
        ${uasat_emscripten_srcs}
        ${CMAKE_CURRENT_SOURCE_DIR}/testing/testing.cpp)
    add_custom_command(OUTPUT testing.js
        COMMAND ${emsripten_prog}
            -std=c++11
            -O2
            -I${CMAKE_CURRENT_SOURCE_DIR}/../include
            -I${CMAKE_CURRENT_SOURCE_DIR}/../src/solvers
            -I${CMAKE_CURRENT_SOURCE_DIR}/../src/solvers/minisat
            ${testing_emscripten_srcs}
            -s "MODULARIZE=1"
            -o testing.js
        DEPENDS ${testing_emscripten_srcs})
    add_custom_target(testing-js ALL DEPENDS testing.js)
endif()
