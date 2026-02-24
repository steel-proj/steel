#include "internal.h"

#include <sys/host_defs.h>
#include <diagnostics/diagnostics.h>
#include <debugging/stack_trace.h>
#include <output/logging/log.h>

#if defined(STEELC_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace {
// windows support only for now
#if defined(STEELC_PLATFORM_WINDOWS)

	bool is_debugger_attached() {
		return IsDebuggerPresent() != 0;
	}
	void try_trigger_debug_break() {
		if (is_debugger_attached()) {
			__debugbreak();
		}
	}

#else

	bool is_debugger_attached() {
		return false;
	}
	void try_trigger_debug_break() {
	}

#endif
}

void s_assert_fail_base(
	const char* expr_str,
	const char* file,
	int line,
	const char* func,
	std::string_view message
) {
	// print diagnostic message
	diagnostics::error("Assertion failed: {}\n", expr_str);
	diagnostics::error("Location: {}:{} in function {}\n", file, line, func);
	if (!message.empty()) {
		diagnostics::error("Message: {}\n", message);
	}
	
	// more detailed internal log
	output::log::print("Internal assertion failure: {}\n", expr_str);
	output::log::print("Location: {}:{} in function {}\n", file, line, func);
	if (!message.empty()) {
		output::log::print("Message: {}\n", message);
	}
	// stack trace
	debugging::stack_trace trace = debugging::stack_trace::capture(2); // skip assert_fail_impl and assert_fail_base
	output::log::print("Stack trace (most recent call first):\n");
	for (const auto& frame : trace.get_frames()) {
		if (!frame.file_name.empty()) {
			output::log::print("  at {} ({}:{})\n", frame.function_name, frame.file_name, frame.line_number);
			continue;
		}

		// no symbol info
		output::log::print("  at {} (0x{:x})\n", frame.function_name, frame.address);
	}

	// try to break
	try_trigger_debug_break();

	// safely exit
	std::exit(INTERNAL_ASSERT_FAIL);
}

void s_fatal_base(const char* file, int line, const char* func, std::string_view message) {
	// print diagnostic message
	diagnostics::error("Fatal error: {}\n", message);
	diagnostics::error("Location: {}:{} in function {}\n", file, line, func);

	// more detailed internal log
	output::log::print("Internal fatal error: {}\n", message);
	output::log::print("Location: {}:{} in function {}\n", file, line, func);
	// stack trace
	debugging::stack_trace trace = debugging::stack_trace::capture(2); // skip fatal_impl and fatal_base
	output::log::print("Stack trace (most recent call first):\n");
	for (const auto& frame : trace.get_frames()) {
		if (!frame.file_name.empty()) {
			output::log::print("  at {} ({}:{})\n", frame.function_name, frame.file_name, frame.line_number);
			continue;
		}

		// no symbol info
		output::log::print("  at {} (0x{:x})\n", frame.function_name, frame.address);
	}

	// try to break
	try_trigger_debug_break();

	// safely exit
	std::exit(INTERNAL_FATAL_ERROR);
}
