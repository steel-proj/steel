#include "router.h"
#include "router.h"

#include <memory>
#include <iostream>
#include <fstream>

#include <output/sinks/stream_sink.h>
#include <output/sinks/file_sink.h>
#include <diagnostics/diagnostics.h>

output::router::router() {
	create_default_sinks();
}

void output::router::create_default_sinks() {
	default_primary_sink = std::make_unique<stream_sink>(std::cout);
	default_diagnostics_sink = std::make_unique<stream_sink>(std::cerr);
	if (!log_file_stream.is_open()) {
		log_file_stream.open("steelc.log", std::ios::app);
	}
	default_log_sink = std::make_unique<file_sink>(log_file_stream);
}

inline ioutput_sink* output::router::get_default_sink(channel channel) {
	switch (channel) {
	case channel::PRIMARY:
		return default_primary_sink.get();
	case channel::DIAGNOSTICS:
		return default_diagnostics_sink.get();
	case channel::LOG:
		return default_log_sink.get();
	default:
		return nullptr;
	}
}
ioutput_sink* output::router::get_channel_sink(channel channel, bool overrides_only) {
	auto it = channel_sinks.find(channel);
	if (it != channel_sinks.end()) {
		return it->second;
	}
	if (!overrides_only) {
		return get_default_sink(channel);
	}
	return nullptr;
}