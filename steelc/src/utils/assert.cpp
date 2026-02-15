#include "assert.h"

#include <cstdlib>
#include <string_view>

#include <output/output.h>

void s_assert_fail_base(
	const char* expr_str,
	const char* file,
	int line,
	const char* func,
	std::string_view message
) {
	// print message + debug info
	output::err("Assertion failed: {}", console_colors::RED, expr_str);
	output::err("Location: {}:{} in function {}", console_colors::RED, file, line, func);

	if (!message.empty()) {
		output::err("Message: {}", console_colors::RED, message);
	}

	// abort the program
	std::abort();
}
