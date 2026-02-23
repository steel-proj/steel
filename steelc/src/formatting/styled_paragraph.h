#pragma once

#include <vector>
#include <string_view>

#include <formatting/styled_string.h>

// styled_paragraph
//
// this class is used for representing a paragraph of styled text.
// it is essentially a collection of styled_string segments, which can be
// concatenated together to form the final output.
//
// it is reccommended to use this class over a std::vector<styled_string>
// since it contains helpful utilities for manipulating and formatting
// the segments.
//
// notes:
// - each segment is not guaranteed to be a single line or word - it can
// be any arbitrary string.

class styled_paragraph {
public:
    styled_paragraph() = default;

    inline const std::vector<styled_string>& get_segments() const {
        return segments;
    }

    inline styled_paragraph& operator<<(const styled_string& ss) {
        segments.push_back(ss);
        return *this;
    }
    inline styled_paragraph& operator<<(styled_string&& ss) {
        segments.push_back(ss);
        return *this;
    }
    inline styled_paragraph& operator<<(const std::string& str) {
        segments.push_back(str);
        return *this;
    }
    inline styled_paragraph& operator<<(std::string&& str) {
        segments.push_back(str);
        return *this;
    }

    // returns the raw paragraph without any styling information
    inline std::string raw() const {
        std::string result;
        for (const auto& segment : segments) {
            result += segment.raw();
        }
        return result;
    }
    // returns a formatted version of the paragraph using the provided style formatter function
    inline std::string format(std::function<std::string(const text_style&)> style_formatter) const {
        std::string result;
        for (const auto& segment : segments) {
            result += segment.format(style_formatter);
        }
        return result;
    }

private:
    std::vector<styled_string> segments;
};