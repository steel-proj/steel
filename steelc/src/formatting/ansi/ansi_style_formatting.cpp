#include "ansi_style_formatting.h"

#include <sstream>
#include <string>

#include <formatting/text_style.h>

std::string ansi_style_formatting::style_to_ansi(const text_style& style) {
    std::ostringstream ss;
    ss << "\033[";

    // add attributes
    if (style.has_attrib(text_style::attribute::BOLD)) {
        ss << "1;";
    }
    if (style.has_attrib(text_style::attribute::DIM)) {
        ss << "2;";
    }
    if (style.has_attrib(text_style::attribute::ITALIC)) {
        ss << "3;";
    }
    if (style.has_attrib(text_style::attribute::UNDERLINE)) {
        ss << "4;";
    }
    if (style.has_attrib(text_style::attribute::BLINK)) {
        ss << "5;";
    }
    if (style.has_attrib(text_style::attribute::REVERSED)) {
        ss << "7;";
    }

    // add foreground + background colors
    const uint8_t fg_code = static_cast<uint8_t>(style.get_fg());
    const uint8_t bg_code = static_cast<uint8_t>(style.get_bg());

    // adjust background code
    uint8_t adjusted_bg;
    if (bg_code >= 90) {
        adjusted_bg = bg_code + 10;  // 90-97 -> 100-107
    } else {
        adjusted_bg = bg_code + 10;  // 30-37 -> 40-47
    }

    ss << static_cast<int>(fg_code) << ";" << static_cast<int>(adjusted_bg) << "m";

    return ss.str();
}