#include "source_context.h"

#include <vector>
#include <algorithm>
#include <string>

#include <formatting/formatting.h>
#include <error/compilation_error.h>
#include <ast/compilation_unit.h>

error_formatting::source_context::source_context(const compilation_error& err) {
	src_file = nullptr;
	context_lines.clear();

	if (err.unit && err.unit->source_file) {
		src_file = err.unit->source_file.get();
		end_line_num = static_cast<int>(err.span.end.line);
		start_line_num = std::max(end_line_num - CONTEXT_SIZE + 1, 1);

		for (int i = start_line_num; i <= end_line_num && i <= static_cast<int>(src_file->lines.size()); ++i) {
			context_lines.push_back(src_file->lines[static_cast<size_t>(i) - 1]);
		}
	}
}

bool error_formatting::source_context::valid() const {
	return src_file != nullptr &&
		!context_lines.empty() &&
		start_line_num > 0 &&
		end_line_num >= start_line_num;
}

std::string error_formatting::source_context::file_path() const {
	return src_file ? formatting::format_path(src_file->relative_path) : "<unknown file>";
}
