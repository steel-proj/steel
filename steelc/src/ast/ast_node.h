#pragma once

#include <memory>
#include <string>

#include <ast/ast_fwd.h>
#include <ast/ast_visitor.h>
#include <lexer/token.h>
#include <utils/iclonable.h>
#include <utils/disable_copy.h>
#include <error/internal.h>

#define ENABLE_ACCEPT_AST(T) void accept(ast_visitor& visitor) override { \
	visitor.visit(*this); \
}

class ast_node : public iclonable<ast_node> {
public:
	// disable copy for all ast nodes as
	// we're heavily dependant on unique_ptr
	DISABLE_COPY(ast_node)
	// shouldn't be possible to clone ast node
	// directly anyway (abstract class) but just to
	// be safe
	DISABLE_CLONE(ast_node, ast_node)

public:
	ast_node() = default;
	virtual ~ast_node() = default;

	virtual std::string string(int indent = 0) const = 0;

	virtual void accept(ast_visitor& visitor) = 0;

	virtual std::unique_ptr<ast_node> clone() const = 0;

	code_span span;

protected:
	virtual void clone_into(ast_node& target) const override {
		target.span = span;
	}

	 std::string indent_s(int level) const {
		return std::string(level * 4, ' ');
	}
};
