#pragma once

#include <string_view>
#include <format>

// This file contains internal error handling utilities for within
// the steel compiler.

#define s_assert(expr, ...) \
    do { \
        if (!(expr)) { \
            s_assert_fail_impl(#expr, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        } \
    } while (0)

#define s_fatal(message, ...) \
	do { \
		s_fatal_impl(__FILE__, __LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

enum internal_error_code {
	INTERNAL_ASSERT_FAIL = 2,
	INTERNAL_FATAL_ERROR = 3,
};

[[noreturn]] void s_assert_fail_base(
	const char* expr_str,
	const char* file,
	int line,
	const char* func,
	std::string_view message
);

template<typename... Args>
[[noreturn]] inline void s_assert_fail_impl(
	const char* expr_str,
	const char* file,
	int line,
	const char* func,
	std::format_string<Args...> fmt,
	Args&&... args
) {
	auto message = std::format(fmt, std::forward<Args>(args)...);
	s_assert_fail_base(expr_str, file, line, func, message);
}

[[noreturn]] void s_fatal_base(
	const char* file,
	int line,
	const char* func,
	std::string_view message
);

template<typename... Args>
[[noreturn]] inline void s_fatal_impl(
	const char* file,
	int line,
	const char* func,
	std::format_string<Args...> fmt,
	Args&&... args
) {
	auto message = std::format(fmt, std::forward<Args>(args)...);
	s_fatal_base(file, line, func, message);
}
