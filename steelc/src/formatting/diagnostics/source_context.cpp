#include "source_context.h"

#include <vector>
#include <algorithm>
#include <string>

#include <formatting/formatting.h>
#include <diagnostics/compilation_diagnostic.h>
#include <ast/compilation_unit.h>

diagnostic_formatting::source_context::source_context(const compilation_diagnostic& diag) {
	src_file = nullptr;
	context_lines.clear();

	if (diag.source) {
		src_file = diag.source;
		end_line_num = static_cast<int>(diag.span.end.line);
		start_line_num = std::max(end_line_num - CONTEXT_SIZE + 1, 1);

		for (int i = start_line_num; i <= end_line_num && i <= static_cast<int>(src_file->lines.size()); ++i) {
			context_lines.push_back(src_file->lines[static_cast<size_t>(i) - 1]);
		}
	}
}

bool diagnostic_formatting::source_context::valid() const {
	return src_file != nullptr &&
		!context_lines.empty() &&
		start_line_num > 0 &&
		end_line_num >= start_line_num;
}

std::string diagnostic_formatting::source_context::file_path() const {
	return src_file ? formatting::format_path(src_file->relative_path) : "<unknown file>";
}
