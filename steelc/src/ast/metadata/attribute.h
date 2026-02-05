#pragma once

#include <string>
#include <memory>
#include <vector>

#include <ast/ast_node.h>

class attribute : public ast_node, public std::enable_shared_from_this<attribute> {
public:
	ENABLE_ACCEPT(attribute)

	attribute(std::string name)
		: name(name) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Attribute: '" + name + "'";
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<attribute>(name);
		cloned->span = span;
		return cloned;
	}

	std::string name;
};