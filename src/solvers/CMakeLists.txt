add_library(uasat-solvers STATIC
    solver.cpp
    minisat.cpp
    minisat/minisat/core/Solver.cc
    minisat/minisat/utils/Options.cc
    minisat/minisat/utils/System.cc)

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
