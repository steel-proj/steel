#pragma once

#include <cstdarg>
#include <memory>
#include <vector>
#include <unordered_set>

#include <lexer/token.h>
#include <parser/modifier.h>
#include <compiler/compilation_step.h>
#include <ast/ast_fwd.h>
#include <symbolics/symbol_error.h>
#include <representations/types/types_fwd.h>

constexpr auto ENABLE_GENERICS = true;

class parser : public compilation_step {
public:
	parser(compilation_ctx& ctx, compilation_unit* unit, std::vector<token>& tokens)
		: compilation_step(ctx), _unit(unit), _tokens(tokens) {
	}

	void parse();
	std::unique_ptr<ast_node> parse_declaration();
	std::unique_ptr<function_declaration> parse_constructor_declaration(token& typename_token);
	std::unique_ptr<function_declaration> parse_function_declaration(bool is_constructor, bool is_override);
	std::unique_ptr<variable_declaration> parse_variable_declaration(bool is_const);
	std::unique_ptr<type_declaration> parse_type_declaration(token& kind_token);
	std::unique_ptr<enum_declaration> parse_enum_declaration();
	std::unique_ptr<variable_declaration> parse_parameter();
	std::unique_ptr<ast_node> parse_statement();
	std::unique_ptr<code_block> parse_block(bool is_body = true);
	std::unique_ptr<if_statement> parse_if_statement();
	std::unique_ptr<for_loop> parse_for_loop();
	std::unique_ptr<while_loop> parse_while_loop();
	std::unique_ptr<return_statement> parse_return_statement();
	std::unique_ptr<break_statement> parse_break_statement();
	std::unique_ptr<ast_node> parse_expression_statement();
	std::unique_ptr<expression> parse_expression();
	std::unique_ptr<expression> parse_binary_expression(int precedence = 0);
	std::unique_ptr<expression> parse_unary_expression();
	std::unique_ptr<expression> parse_primary_expression();
	std::unique_ptr<initializer_list> parse_initializer_list();
	std::unique_ptr<initializer_list> parse_array_initializer();

	// helper functions
	std::vector<std::unique_ptr<attribute>> parse_attributes();
	std::vector<modifier> parse_modifiers();
	type_ptr parse_type();
	std::vector<std::unique_ptr<expression>> parse_expression_list(token_type end = TT_RPAREN);
	std::vector<std::unique_ptr<variable_declaration>> parse_parameter_list(token_type end = TT_RPAREN);
	std::vector<std::unique_ptr<generic_parameter>> parse_generics();

private:
	compilation_unit* _unit;
	std::vector<token>& _tokens;
	size_t _position = 0;

	token& peek();
	token& previous();
	token& consume();
	bool match_primitive();
	bool match(int count, ...);
	bool match(token_type type);
	bool check(token_type type);
	void advance();
	void synchronize();
	bool is_at_end();
};
