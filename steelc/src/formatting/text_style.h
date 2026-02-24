#pragma once

#include <cstdint>
#include <string>

// text_style
// 
// This class encapsulates the style used for a log message.
// It is mostly used for converting to ANSI escape codes and outputting
// stylized messages to the console.

class text_style {
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
        BRIGHT_BLUE = 94,
        BRIGHT_MAGENTA = 95,
        BRIGHT_CYAN = 96,
        BRIGHT_WHITE = 97,
	};

public:
    text_style() = default;
    explicit text_style(color fg, color bg = color::BLACK, attribute attr = attribute::NONE)
        : foreground(fg), background(bg), attributes(static_cast<uint8_t>(attr)) {
        // dont allow implicit - thats what text_styles::colors is for
    }

    static void enable(); // note: only *attempts* to enable
    static bool is_enabled(); // returns true if successfully enabled

    inline text_style& fg(color new_fg) {
        foreground = new_fg;
        return *this;
	}
    inline text_style& bg(color new_bg) {
        background = new_bg;
        return *this;
	}
    inline text_style& attrib(attribute new_attr) {
        attributes |= static_cast<uint8_t>(new_attr);
		return *this;
	}

    inline text_style& bold() {
		return attrib(attribute::BOLD);
    }
    inline text_style& dim() {
		return attrib(attribute::DIM);
	}
	inline text_style& italic() {
		return attrib(attribute::ITALIC);
	}
    inline text_style& underline() {
		return attrib(attribute::UNDERLINE);
	}
	inline text_style& blink() {
		return attrib(attribute::BLINK);
	}
	inline text_style& reversed() {
		return attrib(attribute::REVERSED);
	}

    inline color get_fg() const {
        return foreground;
    }
    inline color get_bg() const {
        return background;
    }
    inline bool has_attrib(attribute attr) const {
        return (attributes & static_cast<uint8_t>(attr)) != 0;
    }

private:
    color foreground = color::WHITE;
    color background = color::BLACK;
    uint8_t attributes = 0;
};

// common reusable text styles
namespace text_styles {
    inline const text_style DEFAULT;
    inline const text_style DIM = text_style().dim();
	inline const text_style ERROR = text_style(text_style::color::RED).bold();
	inline const text_style WARNING = text_style(text_style::color::YELLOW);
    inline const text_style NOTE = text_style(text_style::color::BLUE);
    inline const text_style SUCCESS = text_style(text_style::color::GREEN).bold();
    inline const text_style VERBOSE = text_style().dim();

	// common colors as styles for convenience
	// note: only applies to foreground color, background is left as default (black)
    namespace colors {
		inline const text_style BLACK = text_style(text_style::color::BLACK);
        inline const text_style RED = text_style(text_style::color::RED);
        inline const text_style GREEN = text_style(text_style::color::GREEN);
        inline const text_style YELLOW = text_style(text_style::color::YELLOW);
        inline const text_style BLUE = text_style(text_style::color::BLUE);
        inline const text_style MAGENTA = text_style(text_style::color::MAGENTA);
        inline const text_style CYAN = text_style(text_style::color::CYAN);
		inline const text_style WHITE = text_style(text_style::color::WHITE);
		inline const text_style BRIGHT_BLACK = text_style(text_style::color::BRIGHT_BLACK);
		inline const text_style BRIGHT_RED = text_style(text_style::color::BRIGHT_RED);
        inline const text_style BRIGHT_GREEN = text_style(text_style::color::BRIGHT_GREEN);
        inline const text_style BRIGHT_YELLOW = text_style(text_style::color::BRIGHT_YELLOW);
        inline const text_style BRIGHT_BLUE = text_style(text_style::color::BRIGHT_BLUE);
        inline const text_style BRIGHT_MAGENTA = text_style(text_style::color::BRIGHT_MAGENTA);
		inline const text_style BRIGHT_CYAN = text_style(text_style::color::BRIGHT_CYAN);
		inline const text_style BRIGHT_WHITE = text_style(text_style::color::BRIGHT_WHITE);
    }
}
