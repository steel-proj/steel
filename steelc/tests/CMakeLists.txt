# steelc/tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(steelc_tests LANGUAGES CXX)

add_executable(steelc_tests
	"tests_main.cpp"

	# harness/
	"harness/test.h"
	"harness/test_defs.h"
	"harness/test_manager.cpp"
	"harness/test_manager.h"

	# lexer_tests/
	"lexer_tests/lexer_tests.cpp"	
)

target_link_libraries(steelc_tests PRIVATE
	steelc_core
)
