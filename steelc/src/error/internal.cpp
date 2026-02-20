#include "internal.h"

#include <output/output.h>

void s_assert_fail_base(
	const char* expr_str,
	const char* file,
	int line,
	const char* func,
	std::string_view message
) {
	// print message + debug info
	output::err("Assertion failed: {}\n", console_colors::RED, expr_str);
	output::err("Location: {}:{} in function {}\n", console_colors::RED, file, line, func);

	if (!message.empty()) {
		output::err("Message: {}\n", console_colors::RED, message);
	}

	// safely exit
	std::exit(INTERNAL_ASSERT_FAIL);
}

void s_fatal_base(const char* file, int line, const char* func, std::string_view message) {
	// print message + debug info
	output::err("Fatal error: {}\n", console_colors::RED, message);
	output::err("Location: {}:{} in function {}\n", console_colors::RED, file, line, func);

	// safely exit
	std::exit(INTERNAL_FATAL_ERROR);
}
