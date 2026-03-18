#pragma once

#include <string>
#include <vector>

#include <ast/ast_node.h>
#include <ast/ast_visitor.h>
#include <utils/iclonable.h>

class code_block : public ast_node, public std::enable_shared_from_this<code_block> {
public:
	ENABLE_ACCEPT_AST(code_block)
	ENABLE_CLONE(code_block, ast_node)

public:
	code_block() = default;

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Code block:\n";
		result += ind + " Body:\n";
		if (body.size() <= 0) {
			result += ind + "  <Empty>\n";
			return result;
		}
		for (int i = 0; i < body.size(); i++) {
			auto& stmt = body[i];
			result += stmt->string(indent + 1) + "\n";
		}
		return result;
	}

	std::vector<std::unique_ptr<ast_node>> body;
	bool is_body = false;

protected:
	virtual void clone_into(ast_node& target) const override {
		ast_node::clone_into(target);
		code_block& block_target = ast_cast<code_block&>(target);
		for (const auto& stmt : body) {
			block_target.body.push_back(stmt->clone());
		}
		block_target.is_body = is_body;
	}
};