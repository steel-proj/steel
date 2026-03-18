#pragma once

#include <memory>
#include <vector>
#include <concepts>

#include <compiler/passes/ir_pass.h>
#include <error/internal.h>

struct compilation_ctx;

template<typename IRType>
class pass_manager {
public:
	pass_manager(compilation_ctx& ctx)
		: _ctx(ctx) {
	}

	template<typename PassType>
	void add_pass() requires std::derived_from<PassType, ir_pass<IRType>> {
		_passes.push_back(std::make_unique<PassType>(_ctx));
	}

	inline bool run(IRType& ir) {
		for (auto& pass : _passes) {
			pass->run(ir);

			if (pass->step_failed()) {
				return false;
			}
		}
		return true;
	}

private:
	compilation_ctx& _ctx;
	std::vector<std::unique_ptr<ir_pass<IRType>>> _passes;
};