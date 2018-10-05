add_executable(validate validate.cpp)
target_include_directories(validate PUBLIC ${CMAKE_BINARY_DIR}/include)
target_link_libraries(validate PUBLIC uasat)

if(emsripten_prog)
    list(APPEND validate_emscripten_srcs
        ${uasat_emscripten_srcs}
        ${CMAKE_CURRENT_SOURCE_DIR}/validate.cpp)

    add_custom_command(OUTPUT validate.js
        COMMAND ${emsripten_prog}
            -std=c++11
            -O2
            -I${CMAKE_SOURCE_DIR}/include
            -I${CMAKE_SOURCE_DIR}/src/solvers
            -I${CMAKE_SOURCE_DIR}/src/solvers/minisat
            ${validate_emscripten_srcs}
            -s "MODULARIZE=1"
            -o validate.js
        DEPENDS ${validate_emscripten_srcs})

    add_custom_command(OUTPUT index.html
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/index.html
            ${CMAKE_CURRENT_BINARY_DIR}/index.html
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/index.html)

    add_custom_target(validate-js ALL DEPENDS validate.js index.html)
endif()
