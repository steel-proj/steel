#include "compiler.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include <compiler/compilation_ctx.h>
#include <compiler/passes/pass_manager.h>
#include <stproj/source_file.h>
#include <lexer/lexer.h>
#include <lexer/token.h>
#include <lexer/token_utils.h>
#include <parser/parser.h>
#include <ast/compilation_unit.h>
#include <ast/ast_pass.h>
#include <ast_passes/declaration_collector.h>
#include <ast_passes/import_resolver.h>
#include <ast_passes/type_resolver.h>
#include <ast_passes/name_resolver.h>
#include <ast_passes/type_checker.h>
#include <ast_passes/init_checker.h>
#include <ast_passes/flow_analyzer.h>
#include <ast_lowering/mir_lowerer.h>
#include <mir/mir_fwd.h>
#include <mir/mir_module.h>
#include <mir/pretty/mir_printer.h>
#include <compiler/compile_config.h>
#include <codegen/codegen.h>
#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>
#include <codegen_llvm/llvm_code_generator.h>
#include <diagnostics/diagnostics.h>
#include <output/output.h>

bool compiler::compile(const compile_config& cl_cfg, codegen_config& cg_cfg) {
	compilation_ctx ctx(module_manager, diag_engine);

	for (auto& file : sources) {
		auto unit = std::make_unique<compilation_unit>();
		unit->source_file = std::make_shared<source_file>(file);

		output::print(text_styles::colors::BLUE, "Compiling: ");
		output::print("\'{}\'\n", file.relative_path);

		lexer lexer(ctx, file.content);
		std::vector<token> tokens = lexer.tokenize();

		if (cl_cfg.print_tokens) {
			output::print("Generated tokens:\n");
			for (const auto& token : tokens) {
				std::string type = to_string(token.type);
				std::string value = token.value;

				std::string sline = std::to_string(token.span.start.line);
				std::string scol = std::to_string(token.span.start.column);
				std::string eline = std::to_string(token.span.end.line);
				std::string ecol = std::to_string(token.span.end.column);

				output::print("[ {}, \"{}\", start: ({}, {}), end: ({}, {})]\n", "", type, value, sline, scol, eline, ecol);
			}
			output::print("\n");
		}

		if (lexer.step_failed()) {
			return false;
		}

		parser parser(ctx, unit.get(), tokens);
		parser.parse();

		if (parser.step_failed()) {
			return false;
		}

		// AST creation complete, collect symbols for each file
		// and map to the modules for later symbol resolution
		declaration_collector collector(ctx);
		collector.run(*unit);

		if (collector.step_failed()) {
			return false;
		}

		compilation_units.push_back(std::move(unit));
	}

	pass_manager<compilation_unit> ast_pm(ctx);
	setup_default_ast_passes(ast_pm);

	// after collecting symbols for each file, we can
	// proceed with all other semantic analysis passes
	for (auto& unit : compilation_units) {
		if (!ast_pm.run(*unit)) {
			return false;
		}
	}

	// lower ast to mir
	mir_lowerer mir_lowerer(ctx);
	std::vector<compilation_unit*> units_raw;
	for (const auto& unit : compilation_units) {
		units_raw.push_back(unit.get());
	}
	std::vector<std::unique_ptr<mir_module>> mir_modules = mir_lowerer.lower_all(units_raw);

	if (cl_cfg.print_mir) {
		mir_printer printer;
		for (const auto& mod : mir_modules) {
			std::string mir_text = printer.print_module(*mod);
			output::print("Generated MIR:\n{}\n", mir_text);
		}
	}

	// generate modules for all units
	// its expected that backend and ir_format have been validated before this point
	codegen codegen(std::move(mir_modules), cg_cfg);
	codegen_result = codegen.generate_all();

	return codegen_result.success;
}

void compiler::setup_default_ast_passes(pass_manager<compilation_unit>& pm) {
	pm.add_pass<import_resolver>();
	pm.add_pass<type_resolver>();
	pm.add_pass<name_resolver>();
	pm.add_pass<type_checker>();
	pm.add_pass<init_checker>();
	pm.add_pass<flow_analyzer>();
}

std::vector<std::string> compiler::read_source(std::string& path) {
	std::vector<std::string> lines;
	std::ifstream file(path);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::string nline = "";
			for (char c : line) {
				if (c == '\t') {
					nline += "    ";
				}
				else {
					nline += c;
				}
			}
			lines.push_back(nline);
		}
		file.close();
	}
	else {
		diagnostics::error("Error: Could not open file {}\n", path);
	}
	return lines;
}
