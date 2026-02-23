#include "stack_trace.h"

#include <cstdlib>
#include <vector>

#include <sys/host_defs.h>

#if defined(STEELC_PLATFORM_WINDOWS)

// windows impl
#include <Windows.h>
#include <DbgHelp.h>

namespace {
    std::vector<debugging::stack_frame> capture_without_symbols(void** stack, USHORT captured) {
        // basic fallback that only records addresses in the case symbol
        // resolution fails for whatever reason
		std::vector<debugging::stack_frame> frames;

		for (USHORT i = 0; i < captured; ++i) {
            void* address = stack[i];
            debugging::stack_frame frame;
            frame.address = reinterpret_cast<std::uintptr_t>(address);
            frame.function_name = "<unknown>";
            frame.file_name = "";
            frame.line_number = 0;
			frames.push_back(std::move(frame));
        }

		return frames;
    }
}

debugging::stack_trace debugging::stack_trace::capture(size_t skip) {
    std::vector<stack_frame> frames;

    void* stack[MAX_FRAMES];
    USHORT captured = CaptureStackBackTrace(
        static_cast<DWORD>(skip + 1), // +1 to ignore capture() itself
        MAX_FRAMES,
        stack,
        nullptr
    );

	if (!stack || captured == 0) {
        // shouldn't really be possible but just incase
        return stack_trace(std::move(frames));
    }

    HANDLE process = GetCurrentProcess();
    if (!SymInitialize(process, nullptr, TRUE)) {
		return stack_trace(capture_without_symbols(stack, captured));
    }

    SYMBOL_INFO* symbol = (SYMBOL_INFO*)std::calloc(1, sizeof(SYMBOL_INFO) + 256);
	if (!symbol) {
        // again, this should be extremely rare
        return stack_trace(capture_without_symbols(stack, captured));
    }

    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    IMAGEHLP_LINE64 line_info{};
    line_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    DWORD displacement_line = 0;

    for (USHORT i = 0; i < captured; ++i) {
        DWORD64 address = (DWORD64)(stack[i]);

        stack_frame frame;
		frame.address = address;

        if (SymFromAddr(process, address, nullptr, symbol)) {
            frame.function_name = symbol->Name;
        }
        else {
			frame.function_name = "<unknown>"; // fallback
        }

        if (SymGetLineFromAddr64(process, address, &displacement_line, &line_info)) {
            frame.file_name = line_info.FileName;
            frame.line_number = line_info.LineNumber;
        }
        else {
			frame.file_name = ""; // fallback
			frame.line_number = 0;
        }

        frames.push_back(std::move(frame));
    }

    std::free(symbol);

    return stack_trace(std::move(frames));
}

#elif defined(STEELC_PLATFORM_LINUX) || defined(STEELC_PLATFORM_OSX)

#error "Stack trace capture not yet implemented for this platform"

// linux/osx impl
debugging::stack_trace debugging::stack_trace::capture(size_t skip) {
    // TODO
	return stack_trace(std::vector<stack_frame>{});
}

#endif
