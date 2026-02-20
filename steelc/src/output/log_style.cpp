#include "log_style.h"

#include <sstream>
#include <io.h>

#include <sys/host_defs.h>

#if defined(STEELC_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace {
    bool is_terminal(FILE* stream) {
		return _isatty(_fileno(stream)) != 0;
    }

    bool enable_windows_ansi() {
        // note:
        // silent fail on platforms older than windows 10, but since
        // this is purely stylistic, it's not an actual issue really.
#if defined(STEELC_PLATFORM_WINDOWS)
        HANDLE out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        HANDLE err_handle = GetStdHandle(STD_ERROR_HANDLE);

        if (out_handle == INVALID_HANDLE_VALUE || err_handle == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD out_mode = 0, err_mode = 0;
        if (!GetConsoleMode(out_handle, &out_mode) || !GetConsoleMode(err_handle, &err_mode)) {
            return false;
        }

		// enable ansi escape code processing
        out_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        err_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        if (!SetConsoleMode(out_handle, out_mode) || !SetConsoleMode(err_handle, err_mode)) {
            return false;
        }

        return true;
#else
		return true; // assume native support
#endif
    }
}

void log_style::enable() {
    static bool enabled = false;
    if (enabled) {
        return;
    }

    if (is_terminal(stdout) && is_terminal(stderr)) {
        enabled = enable_windows_ansi();
	}
}

std::string log_style::to_ansi() const {
	std::ostringstream oss;

    std::ostringstream ss;
    ss << "\033[";

    // add attributes
    if (attributes & static_cast<uint8_t>(attribute::BOLD)) {
        ss << "1;";
	}
    if (attributes & static_cast<uint8_t>(attribute::DIM)) {
        ss << "2;";
    }
    if (attributes & static_cast<uint8_t>(attribute::ITALIC)) {
        ss << "3;";
    }
    if (attributes & static_cast<uint8_t>(attribute::UNDERLINE)) {
        ss << "4;";
    }
    if (attributes & static_cast<uint8_t>(attribute::BLINK)) {
        ss << "5;";
    }
    if (attributes & static_cast<uint8_t>(attribute::REVERSED)) {
        ss << "7;";
	}

	// add foreground + background colors
	ss << static_cast<uint8_t>(foreground) << ";" << (static_cast<uint8_t>(background) + 10) << "m";

    return ss.str();
}
