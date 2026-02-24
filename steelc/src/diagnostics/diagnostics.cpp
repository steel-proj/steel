#include "diagnostics.h"

#include <error/compilation_error.h>
#include <error/compilation_advice.h>
#include <formatting/error_formatting/error_formatter.h>

void diagnostics::print_compilation_error(const compilation_error& err) {
    static auto pf = error_formatting::pretty_error_formatter();
    styled_paragraph formatted = pf.format(err);
    output::router::instance().write(output::router::channel::DIAGNOSTICS, formatted);
}
