#include "diagnostics.h"

#include <error/compilation_error.h>
#include <error/compilation_advice.h>
#include <formatting/error_formatting/error_formatter.h>

void diagnostics::print_compilation_errors(const std::vector<compilation_error>& errors) {
    auto pf = error_formatting::pretty_error_formatter();

    for (const auto& err : errors) {
        styled_paragraph formatted = pf.format(err);
        output::router::instance().write(output::router::channel::DIAGNOSTICS, formatted);
    }
}
