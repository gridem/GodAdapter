enable_testing()

include(../cmake/configure.cmake)

file(GLOB ut_srcs *.cpp *.h)

add_executable(tests ${ut_srcs})
target_link_libraries(tests god_adapter)
use_boost_libraries(tests LIBS unit_test_framework)
add_test(tests tests)
