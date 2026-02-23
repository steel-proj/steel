#pragma once

#include <ostream>

#include <formatting/styled_string.h>
#include <formatting/ansi/ansi_style_formatting.h>
#include <output/ioutput_sink.h>

class stream_sink : public ioutput_sink {
public:
	stream_sink(std::ostream& stream)
		: stream(stream) {
	}

	virtual void write(const styled_string& ss) override {
		stream << ss.format(ansi_style_formatting::style_to_ansi) + "\033[0m";
	}

private:
	std::ostream& stream;
};
