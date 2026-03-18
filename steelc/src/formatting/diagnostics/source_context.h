#pragma once

#include <string>
#include <vector>

#include <diagnostics/compilation_diagnostic.h>
#include <stproj/source_file.h>

namespace diagnostic_formatting {
	class source_context {
	public:
		static constexpr int CONTEXT_SIZE = 3;

	public:
		explicit source_context(const compilation_diagnostic& diag);

		bool valid() const;

		// string as it may be "<unknown file>"
		std::string file_path() const;
		const std::vector<std::string>& lines() const { return context_lines; }

		// note: 1-based line numbers
		inline int start_line() const { return start_line_num; }
		inline int end_line() const { return end_line_num; }

	private:
		source_file* src_file;
		std::vector<std::string> context_lines;

		int start_line_num = -1;
		int end_line_num = -1;
	};
}