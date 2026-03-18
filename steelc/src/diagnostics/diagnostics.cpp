#include "diagnostics.h"

#include <diagnostics/compilation_diagnostic.h>
#include <formatting/diagnostics/diagnostic_formatter.h>

void diagnostics::print_diagnostic(const compilation_diagnostic& diag) {
    static auto pf = diagnostic_formatting::pretty_diagnostic_formatter();
    styled_paragraph formatted = pf.format(diag);
    output::router::instance().write(output::router::channel::DIAGNOSTICS, formatted);
}
