add_library(uasat-solvers STATIC
    solver.cpp
    minisat.cpp
    minisat/minisat/core/Solver.cc
    minisat/minisat/utils/Options.cc)

target_include_directories(uasat-solvers PUBLIC ../../include)
target_include_directories(uasat-solvers PRIVATE minisat)

set_target_properties(uasat-solvers PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_features(uasat-solvers
    PUBLIC
      cxx_attributes
      cxx_defaulted_functions
      cxx_deleted_functions
      cxx_final
)

list(APPEND UASAT_EMSCRIPTEN_SOLVER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/solver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/minisat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/minisat/minisat/core/Solver.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/minisat/minisat/utils/Options.cc)
set(UASAT_EMSCRIPTEN_SOLVER_SRCS "${UASAT_EMSCRIPTEN_SOLVER_SRCS}" PARENT_SCOPE)
