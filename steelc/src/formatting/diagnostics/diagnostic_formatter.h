#pragma once

#include <string>

#include <formatting/styled_string.h>
#include <formatting/styled_paragraph.h>
#include <diagnostics/compilation_diagnostic.h>

namespace diagnostic_formatting {
	class diagnostic_formatter {
	public:
		virtual ~diagnostic_formatter() = default;
		virtual styled_paragraph format(const compilation_diagnostic& err) = 0;
	};

	class pretty_diagnostic_formatter : public diagnostic_formatter {
	public:
		styled_paragraph format(const compilation_diagnostic& err) override;
	};
}