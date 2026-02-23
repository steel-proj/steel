#pragma once

#include <string>

#include <formatting/text_style.h>

// ansi_style_formatting
//
// this namespace provides utility functions for formatting
// text using ANSI escape codes.

namespace ansi_style_formatting {
    std::string style_to_ansi(const text_style& style);
}
