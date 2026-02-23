#pragma once

#include <string>

#include <formatting/styled_string.h>
#include <formatting/styled_paragraph.h>
#include <error/compilation_error.h>

namespace error_formatting {
	class error_formatter {
	public:
		virtual ~error_formatter() = default;
		virtual styled_paragraph format(const compilation_error& err) = 0;
	};

	class pretty_error_formatter : public error_formatter {
	public:
		styled_paragraph format(const compilation_error& err) override;
	};
}