#pragma once

#include <cstdlib>
#include <cstdarg>
#include <string_view>
#include <format>

#include <output/output.h>

// assert.h
// 
// this file defines the steel assert mechanism, which is more
// informative and flexible than the default c++ assert.

[[noreturn]] void s_assert_fail_base(
	const char* expr_str,
	const char* file,
	int line,
	const char* func,
	std::string_view message
);

template<typename... Args>
[[noreturn]] inline void s_assert_fail(
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

#define s_assert(expr, ...) \
    do { \
        if (!(expr)) { \
            s_assert_fail(#expr, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        } \
    } while (0)
