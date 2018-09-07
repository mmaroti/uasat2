add_subdirectory(solvers)

add_executable(uasat-test test.cpp)
target_include_directories(uasat-test PUBLIC ../include)
target_link_libraries(uasat-test PUBLIC uasat-solvers)
