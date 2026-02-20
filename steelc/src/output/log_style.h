#pragma once

#include <cstdint>
#include <string>

// log_style
// 
// This class encapsulates the style used for a log message.
// It is mostly used for converting to ANSI escape codes and outputting
// stylized messages to the console.

class log_style {
public:
	enum class attribute : uint8_t {
        NONE = 0,
        BOLD = 1 << 0,
        DIM = 1 << 1,
        ITALIC = 1 << 2,
        UNDERLINE = 1 << 3,
        BLINK = 1 << 4,
        REVERSED = 1 << 5,
	};

	enum class color : uint8_t {
        BLACK = 30,
        RED = 31,
        GREEN = 32,
        YELLOW = 33,
        BLUE = 34,
        MAGENTA = 35,
        CYAN = 36,
        WHITE = 37,
        BRIGHT_BLACK = 90,
        BRIGHT_RED = 91,
        BRIGHT_GREEN = 92,
        BRIGHT_YELLOW = 93,
        BRIGHT_CYAN = 96,
	};

public:
    log_style()
		: foreground(color::WHITE), background(color::BLACK), attributes(0) {
	}
    log_style(color fg, color bg = color::BLACK, attribute attr = attribute::NONE)
        : foreground(fg), background(bg), attributes(static_cast<uint8_t>(attr)) {
    }

    static void enable();

    inline log_style& fg(color new_fg) {
        foreground = new_fg;
        return *this;
	}
    inline log_style& bg(color new_bg) {
        background = new_bg;
        return *this;
	}
    inline log_style& attrib(attribute new_attr) {
        attributes |= static_cast<uint8_t>(new_attr);
		return *this;
	}

    inline log_style& bold() {
		return attrib(attribute::BOLD);
    }
    inline log_style& dim() {
		return attrib(attribute::DIM);
	}
	inline log_style& italic() {
		return attrib(attribute::ITALIC);
	}
    inline log_style& underline() {
		return attrib(attribute::UNDERLINE);
	}
	inline log_style& blink() {
		return attrib(attribute::BLINK);
	}
	inline log_style& reversed() {
		return attrib(attribute::REVERSED);
	}
        
    std::string to_ansi() const;

private:
    color foreground;
    color background;
    uint8_t attributes;
};

namespace log_styles {
    inline const log_style DEFAULT;
	inline const log_style ERROR = log_style(log_style::color::RED).bold();
	inline const log_style WARNING = log_style(log_style::color::YELLOW);
    inline const log_style INFO = log_style(log_style::color::BLUE);
	inline const log_style SUCCESS = log_style(log_style::color::GREEN).bold();
}
