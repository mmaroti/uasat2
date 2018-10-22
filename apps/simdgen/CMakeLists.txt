add_executable(simdgen simdgen.cpp)
target_include_directories(simdgen PUBLIC ${CMAKE_BINARY_DIR}/include)
target_link_libraries(simdgen PUBLIC uasat)
