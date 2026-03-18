#pragma once

#include <compiler/compilation_step.h>

struct compilation_ctx;

template<typename IRType>
class ir_pass : public compilation_step {
public:
	using ir_type = IRType;

public:
	explicit ir_pass(compilation_ctx& ctx)
		: compilation_step(ctx) {
	}

	virtual void run(IRType& ir) = 0;
};
