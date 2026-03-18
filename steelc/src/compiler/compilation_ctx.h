#pragma once

#include <diagnostics/diagnostics_engine.h>
#include <modules/module_manager.h>
#include <compiler/instantiation_worklist.h>

class ast_node;
struct mir_module;

struct compilation_ctx {
	explicit compilation_ctx(module_manager& mod_manager, diagnostics_engine& diagnostics)
		: module_manager(mod_manager), diagnostics(diagnostics) {
	}

	diagnostics_engine& diagnostics;
	module_manager& module_manager;
	instantiation_worklist inst_worklist;
};