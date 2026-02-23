#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>

#include <formatting/styled_string.h>
#include <formatting/styled_paragraph.h>
#include <output/ioutput_sink.h>
#include <output/sinks/stream_sink.h>
#include <output/sinks/file_sink.h>

namespace output {
	class router {
	public:
		enum class channel {
			PRIMARY,
			DIAGNOSTICS,
			LOG
		};

	public:
		inline static router& instance() {
			static router instance;
			return instance;
		}

		inline void redirect_channel(channel channel, ioutput_sink* sink) {
			if (sink == nullptr) {
				channel_sinks.erase(channel);
			}
			channel_sinks[channel] = sink;
		}

		inline void write(channel channel, const styled_string& ss) {
			if (auto sink = get_channel_sink(channel)) {
				sink->write(ss);
			}
		}
		inline void write(channel channel, const styled_paragraph& sp) {
			if (auto sink = get_channel_sink(channel)) {
				for (const auto& segment : sp.get_segments()) {
					sink->write(segment);
				}
			}
		}

	private:
		router();
		router(const router&) = delete;
		router& operator=(const router&) = delete;
		router(router&&) = delete;
		router& operator=(router&&) = delete;

		std::unordered_map<channel, ioutput_sink*> channel_sinks;

		std::unique_ptr<stream_sink> default_primary_sink;
		std::unique_ptr<stream_sink> default_diagnostics_sink;
		std::ofstream log_file_stream;
		std::unique_ptr<file_sink> default_log_sink;

		void create_default_sinks();

		ioutput_sink* get_default_sink(channel channel);
		ioutput_sink* get_channel_sink(channel channel, bool overrides_only = false);
	};
}