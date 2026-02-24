#include "error_formatter.h"

#include <string>
#include <sstream>

#include <error/compilation_error.h>
#include <formatting/formatting.h>
#include <formatting/styled_string.h>
#include <formatting/styled_paragraph.h>
#include <formatting/error_formatting/source_context.h>
#include <formatting/text_style.h>
#include <utils/string_utils.h>
#include <utils/math_utils.h>

styled_paragraph error_formatting::pretty_error_formatter::format(const compilation_error& err) {
	source_context ctx(err);
	styled_paragraph p;

	// pretty format:
	// 
	// ERROR <code>: <message>
	// at: <file>:<line>:<col>
	// <lno> | <context line(s)>
	//         ^~~~ HERE

	// ERROR <code>: <message>
	p << styled_string(text_style(text_style::color::RED).bold(), "ERROR " + err.info.code + ": ");
	p << styled_string(text_styles::colors::RED, err.info.message + "\n");

	// at: <file>:<line>:<col>
	p << "at: ";
	p << styled_string(text_styles::colors::BRIGHT_YELLOW, ctx.file_path());
	p << styled_string(text_styles::DIM, ":") << std::to_string(err.span.start.line);
	p << styled_string(text_styles::DIM, ":") << std::to_string(err.span.start.column) + "\n";

	const auto& lines = ctx.lines();
	int lno_width = math_utils::count_digits(ctx.end_line());

	for (size_t i = 0; i < lines.size(); i++) {
		int lno = i + ctx.start_line(); // line number (1-based)
		std::string lno_str = std::to_string(lno);
		lno_str = string_utils::pad_right(lno_str, (size_t)lno_width) + " | ";
		// ^^ line number string (e.g. "10 | ")

		if (lno >= err.span.start.line && lno <= err.span.end.line) {
			// error line
			// print line number + line (not dimmed)
			p << lno_str;

			// early out if the line is empty (to avoid just printing a random gap)
			if (lines[i].empty()) {
				p << "\n";
				continue;
			}
			p << lines[i] + "\n";

			// print error marker(s)
			p << std::string(lno_width + 3 /* account for ' | ' */, ' ');

			// single-line error
			if (err.span.start.line == err.span.end.line) {
				int squiggle_width = err.span.end.column - err.span.start.column + 1;

				p << std::string(err.span.start.column - 2, ' ');
				p << styled_string(text_styles::colors::RED, string_utils::pad_right("^", squiggle_width, '~') + "\n");
				continue;
			}

			// multi-line error
			size_t line_length = lines[i].size();
			if (lno == err.span.start.line) {
				p << std::string(err.span.start.column - 1, ' ');
				p << styled_string(text_styles::colors::RED, string_utils::pad_right("^", line_length - err.span.start.column + 1, '~') + "\n");
			}
			else if (lno == err.span.end.line) {
				p << styled_string(text_styles::colors::RED, string_utils::pad_left("^", err.span.end.column, '~') + "\n");
			}
			else {
				p << styled_string(text_styles::colors::RED, std::string(line_length, '~') + "\n");
			}
		}
		else { // non-error line
			std::string full = lno_str;
			full += lines[i] + "\n";

			p << styled_string(text_styles::DIM, full);
		}
	}

	return p;
}
