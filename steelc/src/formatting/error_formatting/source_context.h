#pragma once

#include <string>
#include <vector>

#include <error/compilation_error.h>
#include <stproj/source_file.h>

namespace error_formatting {
	class source_context {
	public:
		static constexpr int CONTEXT_SIZE = 2;

	public:
		explicit source_context(const compilation_error& err);

		bool valid() const;

		std::string file_path() const;
		const std::vector<std::string>& lines() const { return context_lines; }

		// note: 1-based line numbers
		inline int start_line() const { return start_line_num; }
		inline int end_line() const { return end_line_num; }

	private:
		source_file* src_file;
		std::vector<std::string> context_lines;

		int start_line_num;
		int end_line_num;
	};
}