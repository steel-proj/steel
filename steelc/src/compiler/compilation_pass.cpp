#include "compilation_pass.h"

void compilation_pass::report_error_token(compilation_error_code code_enum, token tk, ...) {
    va_list args;
    va_start(args, tk);
    add_error(code_enum, tk.span.start, tk.span.end, args);
    va_end(args);
}
void compilation_pass::report_error(compilation_error_code code_enum, code_span span, ...) {
    va_list args;
    va_start(args, span);
    add_error(code_enum, span.start, span.end, args);
    va_end(args);
}

void compilation_pass::report_warning_token(compilation_warning_code code_enum, token tk, ...) {
    va_list args;
    va_start(args, tk);
    add_warning(code_enum, tk.span.start, tk.span.end, args);
    va_end(args);
}
void compilation_pass::report_warning(compilation_warning_code code_enum, code_span span, ...) {
    va_list args;
    va_start(args, span);
    add_warning(code_enum, span.start, span.end, args);
    va_end(args);
}

void compilation_pass::add_advice(compiler_advice_code code_enum, ...) {
    if (last_error == nullptr) {
        return;
    }

    compilation_advice_info info = error_catalog::get_advice_info(code_enum);
	va_list args;
    va_start(args, code_enum);
    info.message = vformat(info.message, args);
    va_end(args);

	last_error->advices.push_back({ last_error, info, });
}

void compilation_pass::add_error(compilation_error_code code_enum, position start, position end, va_list args) {
    compilation_error_info info = error_catalog::get_error_info(code_enum);
    info.message = vformat(info.message, args);
    errors.push_back({ compilation_error_kind::ERROR, info, { start, end }, pass_unit });
	last_error = &errors.back();
}

void compilation_pass::add_warning(compilation_warning_code code_enum, position start, position end, va_list args) {
    compilation_error_info info = error_catalog::get_warning_info(code_enum);
    info.message = vformat(info.message, args);
    warnings.push_back({ compilation_error_kind::WARNING, info, { start, end }, pass_unit });
}

std::string compilation_pass::vformat(std::string fmt, va_list args) {
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt.c_str(), args);
    return std::string(buffer);
}
