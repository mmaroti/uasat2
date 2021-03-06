add_subdirectory(solvers)

add_compile_options(-pedantic -Wsuggest-override)

add_library(uasat SHARED
    solver.cpp
    tensor.cpp
    set.cpp
    group.cpp
    clone.cpp
    bitvec.cpp
    func.cpp
    shape.cpp)

target_include_directories(uasat PUBLIC ../include)
target_link_libraries(uasat uasat-minisat)

set_target_properties(uasat PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_features(uasat
    PUBLIC
      cxx_attributes
      cxx_defaulted_functions
      cxx_deleted_functions
      cxx_final
)

list(APPEND uasat_emscripten_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/solver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tensor.cpp
    ${uasat_emscripten_solvers_srcs})
set(uasat_emscripten_srcs "${uasat_emscripten_srcs}" PARENT_SCOPE)
