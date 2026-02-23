#pragma once

#include <string_view>

#include <formatting/styled_string.h>

class ioutput_sink {
public:
	virtual ~ioutput_sink() = default;

	virtual void write(const styled_string& ss) = 0;
};
