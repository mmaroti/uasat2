add_executable(testing testing.cpp)
target_include_directories(testing PUBLIC ${CMAKE_BINARY_DIR}/include)
target_link_libraries(testing PUBLIC uasat)

if(emsripten_prog)
    list(APPEND testing_emscripten_srcs
        ${uasat_emscripten_srcs}
        ${CMAKE_CURRENT_SOURCE_DIR}/testing.cpp)

    add_custom_command(OUTPUT testing.js
        COMMAND ${emsripten_prog}
            -std=c++11
            -O2
            -I${CMAKE_SOURCE_DIR}/include
            -I${CMAKE_SOURCE_DIR}/src/solvers
            -I${CMAKE_SOURCE_DIR}/src/solvers/minisat
            ${testing_emscripten_srcs}
            -s "MODULARIZE=1"
            -o testing.js
        DEPENDS ${testing_emscripten_srcs})

    add_custom_target(testing-js ALL DEPENDS testing.js)
endif()
