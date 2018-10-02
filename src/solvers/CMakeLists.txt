add_library(uasat-minisat STATIC
    minisat.cpp
    minisat/minisat/core/Solver.cc
    minisat/minisat/simp/SimpSolver.cc
    minisat/minisat/utils/Options.cc)
set_property(TARGET uasat-minisat PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(uasat-minisat PUBLIC ../../include)
target_include_directories(uasat-minisat PRIVATE minisat)

set_target_properties(uasat-minisat PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_features(uasat-minisat
    PUBLIC
      cxx_attributes
      cxx_defaulted_functions
      cxx_deleted_functions
      cxx_final
)

list(APPEND uasat_emscripten_solvers_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/minisat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/minisat/minisat/core/Solver.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/minisat/minisat/utils/Options.cc)
set(uasat_emscripten_solvers_srcs "${uasat_emscripten_solvers_srcs}" PARENT_SCOPE)
