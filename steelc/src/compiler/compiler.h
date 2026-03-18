#pragma once

#include <vector>
#include <string>
#include <memory>

#include <compiler/passes/pass_manager.h>
#include <diagnostics/diagnostics_engine.h>
#include <ast/compilation_unit.h>
#include <modules/module_manager.h>
#include <compiler/compile_config.h>
#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>

class source_file;

class compiler {
public:
	compiler(std::vector<source_file> sources)
		: sources(sources) {
	}

	bool compile(const compile_config& cl_cfg, codegen_config& cg_cfg);

	inline bool failed() const {
		return diag_engine.error_count() > 0;
	}

	inline std::vector<compilation_diagnostic> get_diagnostics() const {
		return diag_engine.get_diagnostics();
	}

	inline const codegen_result& get_result() {
		return codegen_result;
	}

private:
	std::vector<std::unique_ptr<compilation_unit>> compilation_units;

	diagnostics_engine diag_engine;
	module_manager module_manager;

	std::vector<source_file> sources;
	codegen_result codegen_result;

	void setup_default_ast_passes(pass_manager<compilation_unit>& pm);
		
	std::vector<std::string> read_source(std::string& path);
};