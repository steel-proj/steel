#pragma once

#include <fstream>

#include <formatting/styled_string.h>
#include <output/ioutput_sink.h>

class file_sink : public ioutput_sink {
public:
	file_sink(std::ofstream& stream)
		: stream(stream) {
	}

	virtual void write(const styled_string& ss) override {
		stream << ss.raw();
	}

private:
	std::ofstream& stream;
};
