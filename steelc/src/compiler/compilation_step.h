#pragma once

#include <string>
#include <vector>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <ranges>

#include <lexer/token.h>
#include <compiler/compilation_ctx.h>
#include <diagnostics/diagnostics_engine.h>
#include <diagnostics/compilation_diagnostic.h>
#include <diagnostics/compilation_diagnostic_catalog.h>
#include <formatting/formatting.h>
#include <error/internal.h>

#define ERROR_TOKEN(code, tk, ...)          report_error(code, tk.span, ##__VA_ARGS__)
#define ERROR(code, span, ...)              report_error(code, span, ##__VA_ARGS__)
#define WARN_TOKEN(code, tk, ...)           report_warning(code, tk.span, ##__VA_ARGS__)
#define WARN(code, span, ...)               report_warning(code, span, ##__VA_ARGS__)
#define HINT(code, ...)                     add_hint(code, ##__VA_ARGS__)

class compilation_step {
public:
    compilation_step(compilation_ctx& ctx)
        : _ctx(ctx), _diagnostics(ctx.diagnostics) {
    }

    inline bool step_failed() const {
        return _diagnostics.error_count() > 0;
    }

protected:
    compilation_ctx& _ctx;
    source_file* _src = nullptr;

    template<typename... Args>
    inline void report_error(compilation_error_code code, code_span span, Args&&... args) {
        compilation_diagnostic_info info = error_catalog::get_error_info(code);
        info.message = formatting::vformat(info.message, std::forward<Args>(args)...);
        compilation_diagnostic error = {
            .kind = compilation_diagnostic_kind::ERROR,
            .info = info,
            .span = span,
            .step = this,
			.source = _src,
            .hints = {}
        };
        _diagnostics.add_diagnostic(error);
    }

    template<typename... Args>
    inline void report_warning(compilation_warning_code code, code_span span, Args&&... args) {
        compilation_diagnostic_info info = error_catalog::get_warning_info(code);
        info.message = formatting::vformat(info.message, std::forward<Args>(args)...);
        compilation_diagnostic warning = {
            .kind = compilation_diagnostic_kind::WARNING,
            .info = info,
            .span = span,
            .step = this,
			.source = _src,
            .hints = {}
        };
        _diagnostics.add_diagnostic(warning);
    }

    template<typename... Args>
    inline void add_hint(compilation_hint_code code, Args&&... args) {
        s_assert(_last_diagnostic != nullptr,
            "Adding a hint requires a non-hint diagnostic to have been added!");

        compilation_diagnostic_info info = error_catalog::get_hint_info(code);
        info.message = formatting::vformat(info.message, std::forward<Args>(args)...);
        compilation_diagnostic hint = {
            .kind = compilation_diagnostic_kind::HINT,
            .info = info,
            .span = code_span(),
            .step = this,
			.source = nullptr,
            .hints = {}
        };
        _last_diagnostic->hints.push_back(hint);
    }

private:
    diagnostics_engine& _diagnostics;
    compilation_diagnostic* _last_diagnostic = nullptr;
};