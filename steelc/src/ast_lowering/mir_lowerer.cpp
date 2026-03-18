#include "mir_lowerer.h"

#include <vector>
#include <memory>
#include <algorithm>

#include <ast/ast.h>
#include <mir/mir_module.h>
#include <mir/mir_function.h>
#include <mir/mir_block.h>
#include <ast_lowering/mir_lowering_visitor.h>
#include <representations/entities/module_entity.h>

std::vector<std::unique_ptr<mir_module>> mir_lowerer::lower_all(const std::vector<compilation_unit*>& units) {
	lowered.clear();
	func_map.clear();

	// lower each unit
	for (auto& unit : units) {
		auto* mod = get_module(unit);
		lower_functions(unit->declarations, mod);
	}

	// lower instantiated generics
	std::vector<std::shared_ptr<function_declaration>> inst_funcs;
	std::shared_ptr<function_declaration> func;
	while (ctx.inst_worklist.dequeue(func)) {
		if (func->is_generic && !func->is_generic_instance) continue;
		inst_funcs.push_back(func);
	}

	for (auto& inst_func : inst_funcs) {
		auto* mod = get_module(inst_func->owning_unit);
		declare_func(inst_func, mod);
	}
	for (auto& inst_func : inst_funcs) {
		define_func(inst_func);
	}

	// return lowered modules
	std::vector<std::unique_ptr<mir_module>> out;
	out.reserve(lowered.size());
	for (auto& [_, m] : lowered) out.push_back(std::move(m));
	return out;
}

mir_module* mir_lowerer::get_module(const compilation_unit* unit) {
	if (lowered.find(unit->unit_id) != lowered.end()) {
		return lowered[unit->unit_id].get();
	}

	// create new module
	auto mm = std::make_unique<mir_module>();
	mm->meta.src_relpath = unit->source_file->relative_path;
	mm->name = unit->source_file->name();

	lowered[unit->unit_id] = std::move(mm);
	return lowered[unit->unit_id].get();
}

void mir_lowerer::lower_functions(const std::vector<ast_node*>& decls, mir_module* mm) {
	std::vector<std::shared_ptr<function_declaration>> funcs;
	collect_functions(decls, funcs);

	for (auto& func : funcs) {
		if (func->is_generic && !func->is_generic_instance) {
			continue; // skip generic function templates
		}
		declare_func(func, mm);
	}
	for (auto& func : funcs) {
		if (func->is_generic && !func->is_generic_instance) {
			continue;
		}
		define_func(func);
	}
}
void mir_lowerer::collect_functions(const std::vector<ast_node*>& decls, std::vector<std::shared_ptr<function_declaration>>& out) {
	for (auto& decl : decls) {
		if (auto mod = ast_ptr_cast<module_declaration>(decl)) {
			collect_functions(mod->declarations, out);
		}
		else if (auto func = ast_ptr_cast<function_declaration>(decl)) {
			out.push_back(func);
		}
	}
}

mir_function& mir_lowerer::declare_func(const std::shared_ptr<function_declaration>& func, mir_module* mm) {
	name_path name;
	mir_type return_type;
	std::vector<mir_type> generic_args;
	std::vector<mir_function_param> params;
	mir_function_flags flags = MIR_FUNC_NONE;

	// naming
	name.name = func->identifier;
	if (func->parent_module && !func->parent_module->is_global()) {
		name.scopes = func->parent_module->name_path();
	}
	if (func->is_entry_point) {
		flags |= MIR_FUNC_NO_MANGLE;
	}

	// attributes
	for (const auto& attr : func->attributes) {
		if (attr->name == "no_mangle") {
			flags |= MIR_FUNC_NO_MANGLE;
		}
	}

	// generics
	if (func->is_generic && func->is_generic_instance) {
		generic_args.reserve(func->generic_args.size());
		for (const auto& arg : func->generic_args) {
			generic_args.push_back({ arg });
		}
	}

	// params
	return_type = { func->return_type };
	params.reserve(func->parameters.size());
	for (const auto& param : func->parameters) {
		// create values for the parameters here to be used later
		auto pval = mir_value({ param->type }, param->identifier);
		params.push_back(mir_function_param{ param->type, param->identifier, pval });
	}

	if (func->is_abstract()) {
		flags |= MIR_FUNC_NO_BODY;
	}

	auto mf = mir_function::create(
		name,
		return_type,
		generic_args,
		params,
		flags
	);

	func_map[func.get()] = mf.get();
	mm->functions.push_back(std::move(mf));

	return *mm->functions.back();
}
void mir_lowerer::define_func(const std::shared_ptr<function_declaration>& func) {
	auto it = func_map.find(func.get());
	if (it == func_map.end() || !it->second) {
		throw std::runtime_error("Cannot define function: MIR declaration missing");
	}

	mir_function& mf = *it->second;
	if (mf.flags & MIR_FUNC_NO_BODY) {
		return;
	}

	// entry block
	if (mf.empty()) {
		mf.add_block("entry");
	}

	// body - use lowering visitor for statements
	mir_lowering_visitor visitor(mf, func_map);
	func->accept(visitor);
}
