#include "llvm_function_builder.h"

#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

#include <codegen/naming/name_mangler.h>
#include <codegen/constants/codegen_constants.h>
#include <codegen/error/codegen_exception.h>
#include <ast/declarations/function_declaration.h>
#include <ast/declarations/type_declaration.h>

llvm::Function* llvm_function_builder::get_or_build(const mir_function& fn_mir, llvm::Module* module) {
	auto linkage = llvm::Function::ExternalLinkage;
	auto fn_type = get_llvm_fn_type(fn_mir);

	// (mangled) name
	std::string name = mangler.mangle_function(fn_mir);

	llvm::Function* func = module->getFunction(name);
	if (!func) {
		func = llvm::Function::Create(
			/* type */ fn_type,
			/* linkage */ linkage,
			/* name (mangled) */ name,
			/* module */ module
		);

		// name args (only at creation)
		auto arg_iter = func->arg_begin();
		for (const auto& param : fn_mir.params) {
			arg_iter->setName(param.name);
			++arg_iter;
		}
	}

	if (func->getFunctionType() != fn_type) {
		throw codegen_exception("Function re-declaration with different type: " + name);
	}

	return func;
}
llvm::FunctionType* llvm_function_builder::get_llvm_fn_type(const mir_function& fn_mir) {
	auto return_type = type_converter.convert(fn_mir.return_type);

	llvm::FunctionType* fn_type = nullptr;
	if (fn_mir.params.empty()) {
		fn_type = llvm::FunctionType::get(
			/* Return Type */ return_type,
			/* isVarArg */ false
		);
	}
	else {
		std::vector<llvm::Type*> llvm_param_types;
		for (const auto& param : fn_mir.params) {
			llvm_param_types.push_back(type_converter.convert(param.type));
		}
		fn_type = llvm::FunctionType::get(
			/* Return Type */ return_type,
			/* Param Types */ llvm_param_types,
			/* isVarArg */ false
		);
		// in the future, we can add support for variadic functions here
	}

	return fn_type;
}

name_mangler llvm_function_builder::mangler;
