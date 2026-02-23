#pragma once

#include <filesystem>

#include <formatting/styled_string.h>
#include <formatting/styled_paragraph.h>

// std_formatters.h
//
// this file defines all the std::formatter extensions
// for types from the standard library that don't
// have built-in formatters (e.g. std::filesystem::path).
//
// in this case, it's also helpful as we can neaten
// it up using lexically_normal() and ensure clean path
// printing.

namespace std {

    // std::filesystem::path
    template<>
    struct formatter<std::filesystem::path> : formatter<std::string>
    {
        auto format(const std::filesystem::path& p, format_context& ctx) const {
            auto str = p.lexically_normal().u8string();
            return formatter<std::string>::format(std::string(str.begin(), str.end()), ctx);
        }
    };

}