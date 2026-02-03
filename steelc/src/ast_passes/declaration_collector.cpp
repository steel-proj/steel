#include "declaration_collector.h"

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <functional>

#include <ast/ast_fwd.h>
#include <ast/ast.h>
#include <lexer/token_utils.h>
#include <parser/modifier.h>
#include <error/error_catalog.h>
#include <compiler/compilation_pass.h>
#include <modules/module_manager.h>
#include <symbolics/symbol_table.h>
#include <representations/entities/module_entity.h>
#include <representations/entities/entity_ref.h>

void declaration_collector::visit(std::shared_ptr<function_declaration> func_decl) {
	// functions and constructors cannot be nested
	if (current_function || current_constructor) {
		if (func_decl->is_constructor) {
			ERROR(ERR_NESTED_CONSTRUCTOR_NOT_ALLOWED, func_decl->span);
		}
		else {
			ERROR(ERR_NESTED_FUNCTION_NOT_ALLOWED, func_decl->span);
		}
		return;
	}

	// validate modifiers
	modifier_flags flags = MOD_NONE;
	modifier_flags allowed = MOD_EXTERN;
	for (const auto& mod : func_decl->modifier_tokens) {
		if (mod.kind & allowed) {
			if (mod.kind & flags) {
				ERROR(ERR_DUPLICATE_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
			}

			flags |= mod.kind;
		}
		else {
			ERROR(ERR_INVALID_FUNCTION_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
		}
	}
	func_decl->modifier_flags = flags;

	// overrides cant be generic
	if (func_decl->is_generic && func_decl->is_override) {
		ERROR(ERR_OVERRIDE_CANT_BE_GENERIC, func_decl->span);
	}

	// TODO: this should be a type checker variable check
	// void parameters are not allowed in functions & constructors
	for (const auto& param : func_decl->parameters) {
		if (param->type->is_primitive() && param->type->primitive == DT_VOID) {
			ERROR(ERR_PARAM_VOID_TYPE, func_decl->span);
			break;
		}
	}

	// constructor-specific logic
	if (func_decl->is_constructor) {
		// constructors cannot be defined outside of a type declaration
		if (!current_type) {
			ERROR(ERR_CONSTRUCTOR_OUTSIDE_TYPE, func_decl->span);
			return;
		}
		func_decl->return_type = current_type->type();

		// check if constructor is already defined
		for (const auto& existing_constructor : current_type->constructors) {
			if (existing_constructor != func_decl && existing_constructor->parameters == func_decl->parameters) {
				ERROR(ERR_CONSTRUCTOR_OVERLOAD_EXISTS, func_decl->span);
				return;
			}
		}

		current_constructor = func_decl;
		if (func_decl->body) {
			func_decl->body->accept(*this);
		}
		current_constructor = nullptr;
		return;
	}

	// check modifiers
	//if (true) {}

	// add to symbol table
	add_symbol_result res = sym_table->add_symbol(func_decl, current_type ? type_entity::get(current_type) : nullptr);
	if (res.error != SYMBOL_OK) {
		switch (res.error) {
		case SYMBOL_CONFLICTS_WITH_FUNCTION: // in this case it means exact same signature
			ERROR(ERR_FUNCTION_ALREADY_DEFINED, func_decl->span, func_decl->identifier.c_str());
			break;
		case SYMBOL_CANNOT_OVERLOAD_BY_RETURN_TYPE:
			ERROR(ERR_CANNOT_OVERLOAD_BY_RETURN_TYPE, func_decl->span, func_decl->identifier.c_str());
			break;

		case SYMBOL_CONFLICTS_WITH_VARIABLE:
			ERROR(ERR_NAME_CONFLICT, func_decl->span, "function", func_decl->identifier, "variable");
			break;
		case SYMBOL_CONFLICTS_WITH_TYPE:
			ERROR(ERR_NAME_CONFLICT, func_decl->span, "function", func_decl->identifier, "type");
			break;
		case SYMBOL_CONFLICTS_WITH_MODULE:
			ERROR(ERR_NAME_CONFLICT, func_decl->span, "function", func_decl->identifier, "module");
			break;

		default:
			ERROR(ERR_INTERNAL_ERROR, func_decl->span, "Declaration Collector", "Unknown error while adding function symbol");
			break;
		}
	}
	func_decl->func_ref = entity_ref(res.eid);

	// set parent module (even for methods, for now anyway) & owning unit
	func_decl->owning_unit = unit;
	func_decl->parent_module = current_module;

	if (func_decl->body) {
		current_function = func_decl;
		func_decl->body->accept(*this);
		current_function = nullptr;
	}
}
void declaration_collector::visit(std::shared_ptr<variable_declaration> var_decl) {
	// NOTE: only global variables should be picked up here
	// local variables will be handled in a later pass

	// validate modifiers
	modifier_flags flags = MOD_NONE;
	modifier_flags allowed = MOD_NONE;
	for (const auto& mod : var_decl->modifier_tokens) {
		if (mod.kind & allowed) {
			if (mod.kind & flags) {
				ERROR(ERR_DUPLICATE_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
			}

			flags |= mod.kind;
		}
		else {
			ERROR(ERR_INVALID_VARIABLE_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
		}
	}
	var_decl->modifier_flags = flags;

	// no variables should be of type void
	if (var_decl->type->is_primitive() && var_decl->type->primitive == DT_VOID) {
		ERROR(ERR_VARIABLE_VOID_TYPE, var_decl->span);
		return;
	}

	// const variables must have an initializer
	if (var_decl->is_const && !var_decl->initializer) {
		ERROR(ERR_CONST_NO_INITIALIZER, var_decl->span);
		return;
	}

	if (!current_function && !current_constructor && !current_type) {
		add_symbol_result res = sym_table->add_symbol(var_decl);
		// only check for module level errors
		// as this is a module-level variable
		if (res.error != SYMBOL_OK) {
			switch (res.error) {
				case SYMBOL_CONFLICTS_WITH_VARIABLE:
					ERROR(ERR_VARIABLE_ALREADY_DECLARED_SCOPE, var_decl->span, var_decl->identifier.c_str());
					break;
				case SYMBOL_CONFLICTS_WITH_FUNCTION:
					ERROR(ERR_NAME_CONFLICT, var_decl->span, "variable", var_decl->identifier, "function");
					break;
				case SYMBOL_CONFLICTS_WITH_TYPE:
					ERROR(ERR_NAME_CONFLICT, var_decl->span, "variable", var_decl->identifier, "type");
					break;
				case SYMBOL_CONFLICTS_WITH_ENUM:
					ERROR(ERR_NAME_CONFLICT, var_decl->span, "variable", var_decl->identifier, "enum");
					break;
				case SYMBOL_CONFLICTS_WITH_MODULE:
					ERROR(ERR_NAME_CONFLICT, var_decl->span, "variable", var_decl->identifier, "module");
					break;

				default:
					ERROR(ERR_INTERNAL_ERROR, var_decl->span, "Declaration Collector", "Unknown error while adding variable symbol");
					break;
			}
		}
		var_decl->var_ref = entity_ref(res.eid);
	}

	// set parent module & owning unit
	var_decl->owning_unit = unit;
	var_decl->parent_module = current_module;
}
void declaration_collector::visit(std::shared_ptr<type_declaration> type_decl) {
	// validate modifiers
	modifier_flags flags = MOD_NONE;
	modifier_flags allowed;
	if (type_decl->type_kind == CT_CLASS) {
		allowed = MOD_NONE;
	} else if (type_decl->type_kind == CT_STRUCT) {
		allowed = MOD_NONE;
	} else if (type_decl->type_kind == CT_INTERFACE) {
		allowed = MOD_NONE;
	}
	for (const auto& mod : type_decl->modifier_tokens) {
		if (mod.kind & allowed) {
			if (mod.kind & flags) {
				ERROR(ERR_DUPLICATE_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
			}

			flags |= mod.kind;
		}
		else {
			error_code err = type_decl->type_kind == CT_CLASS ? ERR_INVALID_CLASS_MODIFIER :
				type_decl->type_kind == CT_STRUCT ? ERR_INVALID_STRUCT_MODIFIER :
				ERR_INVALID_INTERFACE_MODIFIER;
			ERROR(err, mod.tk.span, to_string(mod.tk.type).c_str());
		}
	}
	type_decl->modifier_flags = flags;

	// check if type is already defined
	add_symbol_result res = sym_table->add_symbol(type_decl);
	if (res.error != SYMBOL_OK) {
		switch (res.error) {
			case SYMBOL_CONFLICTS_WITH_TYPE:
				ERROR(ERR_TYPE_ALREADY_DEFINED, type_decl->span, type_decl->name().c_str());
				break;

			case SYMBOL_CONFLICTS_WITH_VARIABLE:
				ERROR(ERR_NAME_CONFLICT, type_decl->span, "type", type_decl->name(), "variable");
				break;
			case SYMBOL_CONFLICTS_WITH_FUNCTION:
				ERROR(ERR_NAME_CONFLICT, type_decl->span, "type", type_decl->name(), "function");
				break;
			case SYMBOL_CONFLICTS_WITH_ENUM:
				ERROR(ERR_NAME_CONFLICT, type_decl->span, "type", type_decl->name(), "enum");
				break;
			case SYMBOL_CONFLICTS_WITH_MODULE:
				ERROR(ERR_NAME_CONFLICT, type_decl->span, "type", type_decl->name(), "module");
				break;

			default:
				ERROR(ERR_INTERNAL_ERROR, type_decl->span, "Declaration Collector", "Unknown error while adding type symbol");
				break;
		}
	}

	// types cannot be nested (yet)
	if (current_type || current_function || current_constructor) {
		ERROR(ERR_NESTED_TYPE_DECLARATION_NOT_ALLOWED, type_decl->span);
		return;
	}

	// structs cannot contain methods
	if (type_decl->type_kind == CT_STRUCT && !type_decl->methods.empty()) {
		ERROR(ERR_STRUCT_CONTAINS_METHOD, type_decl->span);
		return;
	}

	// structs and interfaces cannot be derived
	if (type_decl->type_kind == CT_STRUCT && !type_decl->base_types.empty()) {
		ERROR(ERR_STRUCT_IS_DERIVED, type_decl->span);
		return;
	}
	else if (type_decl->type_kind == CT_INTERFACE && !type_decl->base_types.empty()) {
		ERROR(ERR_INTERFACE_IS_DERIVED, type_decl->span);
		return;
	}

	// interfaces cannot contain constructors or member variables
	if (type_decl->type_kind == CT_INTERFACE && !type_decl->constructors.empty()) {
		ERROR(ERR_INTERFACE_CONTAINS_CONSTRUCTOR, type_decl->span);
		return;
	}
	if (type_decl->type_kind == CT_INTERFACE && !type_decl->fields.empty()) {
		ERROR(ERR_INTERFACE_CONTAINS_MEMBER_VAR, type_decl->span);
		return;
	}

	// set parent module (may be null) & owning unit
	type_decl->owning_unit = unit;
	type_decl->parent_module = current_module;

	current_type = type_decl;
	for (const auto& member : type_decl->fields) {
		member->accept(*this);
	}
	for (const auto& constructor : type_decl->constructors) {
		// this is done during parsing (whether it should or not idk but ill leave it for now)
		//constructor->is_constructor = true;
		constructor->accept(*this);
	}
	for (const auto& method : type_decl->methods) {
		method->is_method = true;
		method->parent_type = type_decl;
		method->accept(*this);
	}
	for (const auto& op : type_decl->operators) {
		op->accept(*this);
	}
	current_type = nullptr;
}
void declaration_collector::visit(std::shared_ptr<module_declaration> mod_decl) {
	std::vector<std::string> module_path;

	// validate modifiers
	modifier_flags flags = MOD_NONE;
	modifier_flags allowed = MOD_NONE;
	for (const auto& mod : mod_decl->modifier_tokens) {
		if (mod.kind & allowed) {
			if (mod.kind & flags) {
				ERROR(ERR_DUPLICATE_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
			}

			flags |= mod.kind;
		}
		else {
			ERROR(ERR_INVALID_MODULE_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
		}
	}
	mod_decl->modifier_flags = flags;

	// identify full module name
	if (!current_module->is_global()) {
		module_path = current_module->name_path();
	}
	module_path.push_back(mod_decl->name);

	// set owning unit (of this specific declaration node)
	mod_decl->owning_unit = unit;

	// set the parent module (again, only for this specific declaration node)
	// since technically, actual modules may be shared across multiple files
	// the node is more of a reference
	mod_decl->parent_module = current_module;

	// lookup module
	auto module_ent = module_manager.get_module(module_path);
	if (module_ent != nullptr) {
		current_module = module_ent;
	}
	else {
		// doesnt exist - create a new module
		// note: dont pass full name, its automatically handled based on the parent module passed
		std::shared_ptr<module_entity> parent = current_module;
		current_module = module_manager.add_module(mod_decl->name, current_module);
		if (parent) {
			// add as submodule
			parent->symbols().add_symbol(current_module);
		}
	}
	sym_table = &current_module->symbols();
	mod_decl->entity = current_module;

	for (const auto& decl : mod_decl->declarations) {
		decl->accept(*this);
	}

	current_module = current_module->parent_module;
	sym_table = &current_module->symbols();
}
void declaration_collector::visit(std::shared_ptr<enum_declaration> enum_decl) {
	// ensure all options reference the owner
	for (auto& option : enum_decl->options) {
		option->declaration = enum_decl;
	}

	// validate modifiers
	modifier_flags flags = MOD_NONE;
	modifier_flags allowed = MOD_NONE;
	for (const auto& mod : enum_decl->modifier_tokens) {
		if (mod.kind & allowed) {
			if (mod.kind & flags) {
				ERROR(ERR_DUPLICATE_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
			}

			flags |= mod.kind;
		}
		else {
			ERROR(ERR_INVALID_ENUM_MODIFIER, mod.tk.span, to_string(mod.tk.type).c_str());
		}
	}
	enum_decl->modifier_flags = flags;

	// add to symbol table
	add_symbol_result res = sym_table->add_symbol(enum_decl);
	if (res.error != SYMBOL_OK) {
		switch (res.error) {
		case SYMBOL_CONFLICTS_WITH_ENUM:
			ERROR(ERR_ENUM_ALREADY_DEFINED, enum_decl->span, enum_decl->identifier);
			break;

		case SYMBOL_CONFLICTS_WITH_VARIABLE:
			ERROR(ERR_NAME_CONFLICT, enum_decl->span, "enum", enum_decl->identifier, "variable");
			break;
		case SYMBOL_CONFLICTS_WITH_FUNCTION:
			ERROR(ERR_NAME_CONFLICT, enum_decl->span, "enum", enum_decl->identifier, "function");
			break;
		case SYMBOL_CONFLICTS_WITH_TYPE:
			ERROR(ERR_NAME_CONFLICT, enum_decl->span, "enum", enum_decl->identifier, "type");
			break;
		case SYMBOL_CONFLICTS_WITH_MODULE:
			ERROR(ERR_NAME_CONFLICT, enum_decl->span, "enum", enum_decl->identifier, "module");
			break;

		default:
			ERROR(ERR_INTERNAL_ERROR, enum_decl->span, "Declaration Collector", "Unknown error while adding enum symbol");
			break;
		}
	}

	// enums cannot be nested
	if (current_type || current_function || current_constructor) {
		ERROR(ERR_NESTED_ENUM_NOT_ALLOWED, enum_decl->span);
		return;
	}

	// cannot have more than one option with the same identifier
	std::unordered_set<std::string> option_names;
	for (const auto& option : enum_decl->options) {
		if (option_names.find(option->identifier) != option_names.end()) {
			ERROR(ERR_ENUM_OPTION_ALREADY_DEFINED, option->span, option->identifier.c_str(), enum_decl->name().c_str());
			return;
		}
		option_names.insert(option->identifier);
	}

	// set parent module & owning unit
	enum_decl->owning_unit = unit;
	enum_decl->parent_module = current_module;
}

void declaration_collector::visit(std::shared_ptr<import_statement> import_stmt) {
	// ensure top-level
	if (!current_module->is_global() || current_type || current_function || current_constructor || !sym_table->in_global_scope()) {
		ERROR(ERR_IMPORT_BAD_POS, import_stmt->span);
		return;
	}

	// everythings OK, do nothing
	// actually resolution occurs in the import_resolver pass
	// we only check here because we have more location context
}
