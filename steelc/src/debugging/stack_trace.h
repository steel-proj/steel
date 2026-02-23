#pragma once

#include <string>
#include <vector>

namespace debugging {
	struct stack_frame {
		std::uintptr_t address;

		std::string function_name;
		std::string file_name;
		int line_number;
	};

	class stack_trace {
	public:
		static constexpr size_t MAX_FRAMES = 64;

	public:
		// captures the current stack trace
		// skip can be used to skip capturing internal frames (e.g. the capture function itself)
		static stack_trace capture(size_t skip = 0);

		const std::vector<stack_frame>& get_frames() const {
			return frames;
		}

	private:
		explicit stack_trace(std::vector<stack_frame>&& frames)
			: frames(std::move(frames)) {
		}

		std::vector<stack_frame> frames;
	};
}
