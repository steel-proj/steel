#pragma once

#include <string>
#include <memory>

#include <parser/modifier.h>
#include <ast/ast_fwd.h>
#include <ast/ast_node.h>
#include <ast/ast_utils.h>
#include <ast/metadata/attribute.h>
#include <representations/entities/entities_fwd.h>
#include <utils/iclonable.h>
#include <utils/disable_copy.h>
#include <error/internal.h>

class compilation_unit;

class declaration : public ast_node {
public:
	DISABLE_CLONE(declaration, ast_node)

public:
	declaration() = default;

	compilation_unit* owning_unit = nullptr;
	module_entity* parent_module = nullptr;
	std::vector<std::unique_ptr<attribute>> attributes;
	std::vector<modifier> modifier_tokens;
	modifier_flags modifier_flags = MOD_NONE;
	std::string filename;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		declaration& decl_target = ast_cast<declaration&>(target);
		decl_target.owning_unit = owning_unit;
		decl_target.parent_module = parent_module;
		for (const auto& attr : attributes) {
			decl_target.attributes.emplace_back(
				ast_cast<attribute>(attr->clone()));
		}
		decl_target.modifier_tokens = modifier_tokens;
		decl_target.modifier_flags = modifier_flags;
		decl_target.filename = filename;
	}
};