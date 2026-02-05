#pragma once

#include <string>
#include <memory>

#include <parser/modifier.h>
#include <ast/ast_node.h>
#include <ast/metadata/attribute.h>
#include <representations/entities/entities_fwd.h>

class compilation_unit;

class declaration : public ast_node {
public:
	std::weak_ptr<compilation_unit> owning_unit;
	std::shared_ptr<module_entity> parent_module = nullptr;
	std::vector<std::shared_ptr<attribute>> attributes;
	std::vector<modifier> modifier_tokens;
	modifier_flags modifier_flags = MOD_NONE;
	std::string filename;
};