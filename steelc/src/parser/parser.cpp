#include "parser.h"

#include <lexer/token_utils.h>
#include <parser/modifier.h>
#include <parser/parser_utils.h>
#include <ast/ast.h>
#include <diagnostics/compilation_diagnostic_catalog.h>

void parser::parse() {
	while (!is_at_end()) {
		auto decl = parse_declaration();
		if (decl) {
			_unit->declarations.emplace_back(std::move(decl));
		}
		else {
			// error occurred, synchronize
			synchronize();
		}
	}
}
std::unique_ptr<ast_node> parser::parse_declaration() {
	auto attrs = parse_attributes();
	auto mods = parse_modifiers();

	std::unique_ptr<declaration> decl = nullptr;

	// module declaration
	if (match(TT_MODULE)) {
		if (!match(TT_IDENTIFIER)) {
			ERROR_TOKEN(ERR_MODULE_NAME_EXPECTED, peek());
			return nullptr;
		}
		token& module_name_token = previous();

		if (!match(TT_LBRACE)) {
			ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
			return nullptr;
		}

		std::vector<std::unique_ptr<ast_node>> mod_declarations;
		while (!is_at_end() && !check(TT_RBRACE)) {
			auto mod_decl = parse_declaration();
			if (mod_decl) {
				mod_declarations.emplace_back(std::move(mod_decl));
			}
			else {
				// error occurred, synchronize
				synchronize();
			}
		}
		if (!match(TT_RBRACE)) {
			ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
			return nullptr;
		}

		decl = make_ast<module_declaration>(module_name_token, module_name_token.value, std::move(mod_declarations));
	}
	// module import
	else if (match(TT_IMPORT)) {
		std::vector<std::string> module_path;
		token& module_token = peek();

		do {
			if (!match(TT_IDENTIFIER)) {
				ERROR_TOKEN(ERR_MODULE_NAME_EXPECTED, peek());
				return nullptr;
			}

			module_path.push_back(previous().value);
		} while (match(TT_SCOPE));

		if (!match(TT_SEMICOLON)) {
			ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
			return nullptr;
		}

		// TODO: not technically even a declaration - i should move this
		return make_ast<import_statement>(module_token, module_path);
	}
	// type declaration
	else if (match(3, TT_STRUCT, TT_CLASS, TT_INTERFACE)) {
		token& kind_token = previous();
		decl = parse_type_declaration(kind_token);
	}
	// enum declaration
	else if (match(TT_ENUM)) {
		decl = parse_enum_declaration();
	}
	// function declaration
	else if (match(TT_FUNC) || match(TT_OVERRIDE)) {
		decl = parse_function_declaration(false, previous().type == TT_OVERRIDE);
	}
	// variable declaration
	else if (match(TT_CONST) || match(TT_LET)) {
		bool is_const = previous().type == TT_CONST;
		decl = parse_variable_declaration(is_const);
	}
	else {
		ERROR_TOKEN(ERR_DECLARATION_EXPECTED, peek());
	}

	if (decl) {
		decl->attributes = std::move(attrs);
		decl->modifier_tokens = mods;
	}

	return decl;
}
std::unique_ptr<function_declaration> parser::parse_constructor_declaration(token& typename_token) {
	auto func_decl = parse_function_declaration(true, false);
	if (func_decl) {
		func_decl->identifier = typename_token.value;
		func_decl->is_constructor = true;
		return func_decl;
	}
	return nullptr;
}
std::unique_ptr<function_declaration> parser::parse_function_declaration(bool is_constructor, bool is_override) {
	token identifier_token;
	if (!is_constructor) {
		if (!match(TT_IDENTIFIER)) {
			ERROR_TOKEN(ERR_ID_EXPECTED, peek());
			return nullptr;
		}
		identifier_token = previous();
	}
	else {
		// constructors dont have identifiers, so we can use the 'constructor' token
		identifier_token = previous();
	}

	std::vector<std::unique_ptr<generic_parameter>> generics;
	if (ENABLE_GENERICS) {
		if (check(TT_LESS)) {
			generics = parse_generics();
			if (generics.empty()) {
				ERROR_TOKEN(ERR_ONE_GENERIC_REQUIRED, previous());
			}
		}
	}

	if (!match(TT_LPAREN)) {
		ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto parameters = parse_parameter_list(TT_RPAREN);

	if (!match(TT_RPAREN)) {
		ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
		return nullptr;
	}

	type_ptr ret_type = to_data_type(DT_VOID);
	if (!is_override && !is_constructor) {
		if (match(TT_ARROW)) {
			ret_type = parse_type();
			if (!ret_type) {
				ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
				return nullptr;
			}
		}
	}

	if (match(TT_SEMICOLON)) {
		return make_ast<function_declaration>(identifier_token, ret_type, identifier_token.value, std::move(parameters), nullptr, is_override);
	}

	auto body = parse_block();
	if (body) {
		auto decl = make_ast<function_declaration>(identifier_token, ret_type, identifier_token.value, std::move(parameters), std::move(body), is_override);
		if (!generics.empty()) {
			decl->is_generic = true;
			decl->generics = std::move(generics);
		}
		return decl;
	}
	return nullptr;
}
std::unique_ptr<variable_declaration> parser::parse_variable_declaration(bool is_const) {
	if (!match(TT_IDENTIFIER)) {
		ERROR_TOKEN(ERR_VAR_NAME_EXPECTED, peek());
		return nullptr;
	}
	token& identifier_token = previous();

	type_ptr var_type = data_type::UNKNOWN;
	if (match(TT_COLON)) {
		var_type = parse_type();
		if (!var_type) {
			ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
			return nullptr;
		}
	}

	std::unique_ptr<expression> initializer = nullptr;
	if (match(TT_ASSIGN)) {
		if (check(TT_LBRACE)) {
			initializer = parse_initializer_list();
		}
		else if (check(TT_LBRACKET)) {
			initializer = parse_array_initializer();
		}
		else {
			initializer = parse_expression();
		}
	}
	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}

	auto decl = make_ast<variable_declaration>(identifier_token, is_const, var_type, identifier_token.value, std::move(initializer));
	return decl;
}
std::unique_ptr<type_declaration> parser::parse_type_declaration(token& kind_token) {
	if (!match(TT_IDENTIFIER)) {
		ERROR_TOKEN(ERR_ID_EXPECTED, peek());
		return nullptr;
	}
	token& identifier_token = previous();

	custom_type_type type_kind = kind_token.type == TT_STRUCT ? CT_STRUCT : kind_token.type == TT_CLASS ? CT_CLASS : CT_INTERFACE;
	auto type_decl = make_ast<type_declaration>(identifier_token, type_kind, identifier_token.value);

	if (ENABLE_GENERICS && match(TT_NOT)) {
		type_decl->generics = parse_generics();
		type_decl->is_generic = true;
	}

	std::vector<type_ptr> base_types;
	if (match(TT_COLON)) {
		do {
			if (!match(TT_IDENTIFIER)) {
				ERROR_TOKEN(ERR_ID_EXPECTED, peek());
				return nullptr;
			}
			auto type = to_data_type(previous());
			base_types.push_back(type);
		} while (match(TT_COMMA));
	}
	type_decl->base_types = base_types;

	if (!match(TT_LBRACE)) {
		ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
		return nullptr;
	}

	size_t current_field_index = 0;
	while (!is_at_end() && !check(TT_RBRACE)) {
		if (match(TT_CONSTRUCTOR)) {
			auto constructor = parse_constructor_declaration(identifier_token);
			if (constructor) type_decl->constructors.emplace_back(std::move(constructor));
			else {
				synchronize();
			}
		}
		else if (match(TT_FUNC) || match(TT_OVERRIDE)) {
			auto method = parse_function_declaration(false, previous().type == TT_OVERRIDE);
			if (method) type_decl->methods.emplace_back(std::move(method));
			else {
				synchronize();
			}
		}
		else if (match(TT_IDENTIFIER)) {
			// field declaration - might change this later
			token& field_name_token = previous();

			if (!match(TT_COLON)) {
				ERROR_TOKEN(ERR_COLON_EXPECTED, peek());
				synchronize();
				continue;
			}

			type_ptr field_type = parse_type();
			if (!field_type) {
				ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
				synchronize();
				continue;
			}

			if (!match(TT_SEMICOLON)) {
				ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
				synchronize();
				continue;
			}

			auto field = make_ast<variable_declaration>(field_name_token, /* not const */ false, field_type, field_name_token.value);
			field->is_field = true;
			field->field_index = current_field_index++;
			type_decl->fields.emplace_back(std::move(field));
		}
		else {
			ERROR_TOKEN(ERR_MEMBER_DECLARATION_EXPECTED, peek());
			advance();
		}
	}
	if (!match(TT_RBRACE)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}

	return type_decl;
}
std::unique_ptr<enum_declaration> parser::parse_enum_declaration() {
	token& identifier_token = peek();
	if (!match(TT_IDENTIFIER)) {
		ERROR_TOKEN(ERR_ID_EXPECTED, peek());
		return nullptr;
	}

	if (!match(TT_LBRACE)) {
		ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
		return nullptr;
	}

	std::vector<std::unique_ptr<enum_option>> options;
	while (!is_at_end() && !check(TT_RBRACE)) {
		if (!match(TT_IDENTIFIER)) {
			ERROR_TOKEN(ERR_ENUM_OPTION_NAME_EXPECTED, peek());
			synchronize();
			continue;
		}
		token& option_name_token = previous();

		auto option = make_ast<enum_option>(option_name_token.value);

		options.emplace_back(std::move(option));

		if (!match(TT_COMMA)) {
			break;
		}
	}
	if (!match(TT_RBRACE)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}

	return make_ast<enum_declaration>(identifier_token, identifier_token.value, std::move(options));
}
std::unique_ptr<variable_declaration> parser::parse_parameter() {
	// parse parameter name
	if (!match(TT_IDENTIFIER)) {
		ERROR_TOKEN(ERR_PARAM_NAME_EXPECTED, peek());
		return nullptr;
	}
	token& param_name_token = previous();

	// parse parameter type
	if (!match(TT_COLON)) {
		ERROR_TOKEN(ERR_COLON_EXPECTED, peek());
		return nullptr;
	}
	type_ptr param_type = parse_type();
	if (!param_type) {
		ERROR_TOKEN(ERR_PARAM_TYPE_EXPECTED, peek());
		return nullptr;
	}

	// TODO: allow for default values in parameters

	auto param = make_ast<variable_declaration>(param_name_token, false, param_type, param_name_token.value);
	param->is_parameter = true;
	return param;
}
std::unique_ptr<ast_node> parser::parse_statement() {
	if (check(TT_LBRACE)) {
		// parse block also matches LBRACE, so we only check here
		return parse_block(false);
	}
	else if (match(TT_IF)) {
		return parse_if_statement();
	}
	else if (match(TT_FOR)) {
		return parse_for_loop();
	}
	else if (match(TT_WHILE)) {
		return parse_while_loop();
	}
	else if (match(TT_RETURN)) {
		return parse_return_statement();
	}
	else if (match(TT_BREAK)) {
		return parse_break_statement();
	}
	return parse_expression_statement();
}
std::unique_ptr<code_block> parser::parse_block(bool is_body) {
	if (!match(TT_LBRACE)) {
		ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
		return nullptr;
	}
	auto block = std::make_unique<code_block>();
	while (!is_at_end() && !check(TT_RBRACE)) {
		if (match(TT_CONST) || match(TT_LET)) {
			bool is_const = previous().type == TT_CONST;
			auto decl = parse_variable_declaration(is_const);
			if (decl) block->body.emplace_back(std::move(decl));
			else {
				synchronize();
			}
		}
		else {
			auto stmt = parse_statement();
			if (stmt) block->body.emplace_back(std::move(stmt));
			else {
				synchronize();
			}
		}
	}
	if (!match(TT_RBRACE)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}
	block->is_body = is_body;
	return block;
}
std::unique_ptr<if_statement> parser::parse_if_statement() {
	token& if_token = previous();
	auto if_stmt = make_ast<if_statement>(if_token);
	if (!match(TT_LPAREN)) {
		ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto condition = parse_expression();
	if_stmt->condition = std::move(condition);
	if (!match(TT_RPAREN)) {
		ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto then_block = parse_block();
	if (then_block) if_stmt->then_block = std::move(then_block);

	if (match(TT_ELSE)) {
		if (match(TT_IF)) {
			// else if statement
			if_stmt->else_node = parse_if_statement();
		}
		else {
			// else block
			if_stmt->else_node = parse_block();
		}
	}

	return if_stmt;
}
std::unique_ptr<for_loop> parser::parse_for_loop() {
	token& for_token = previous();
	if (!match(TT_LPAREN)) {
		ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
		return nullptr;
	}

	std::unique_ptr<ast_node> initializer = nullptr;
	if (match(TT_CONST) || match(TT_LET)) {
		bool is_const = previous().type == TT_CONST;
		initializer = parse_variable_declaration(is_const);
	}
	else if (!check(TT_SEMICOLON)) {
		initializer = parse_expression_statement();
	}
	else {
		// no initializer
		match(TT_SEMICOLON);
	}

	std::unique_ptr<expression> condition = nullptr;
	if (!check(TT_SEMICOLON)) {
		condition = parse_expression();
	}
	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}

	std::unique_ptr<expression> increment = nullptr;
	if (!check(TT_RPAREN)) {
		increment = parse_expression();
	}
	if (!match(TT_RPAREN)) {
		ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto body = parse_block();

	return make_ast<for_loop>(for_token, std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}
std::unique_ptr<while_loop> parser::parse_while_loop() {
	token& while_token = previous();
	if (!match(TT_LPAREN)) {
		ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto condition = parse_expression();
	if (!match(TT_RPAREN)) {
		ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto body = parse_block();

	return make_ast<while_loop>(while_token, std::move(condition), std::move(body));
}
std::unique_ptr<return_statement> parser::parse_return_statement() {
	token& return_token = previous();

	// base return
	if (match(TT_SEMICOLON)) {
		return make_ast<return_statement>(return_token, nullptr, nullptr);
	}

	// returns value?
	std::unique_ptr<expression> value = nullptr;
	if (!check(TT_IF) && !check(TT_SEMICOLON)) {
		value = parse_expression();
	}

	// is conditional?
	std::unique_ptr<expression> condition = nullptr;
	if (match(TT_IF)) {
		condition = parse_expression();
	}

	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}

	return make_ast<return_statement>(return_token, std::move(value), std::move(condition));
}
std::unique_ptr<break_statement> parser::parse_break_statement() {
	token& break_token = previous();

	// is conditional?
	std::unique_ptr<expression> condition = nullptr;
	if (match(TT_IF)) {
		condition = parse_expression();
	}

	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}

	return make_ast<break_statement>(break_token, std::move(condition));
}
std::unique_ptr<ast_node> parser::parse_expression_statement() {
	token& expr_token = peek();
	auto expr = parse_expression();
	if (match(TT_IF)) {
		auto cond = parse_expression();
		if (!match(TT_SEMICOLON)) {
			ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
			return nullptr;
		}
		return make_ast<inline_if>(expr_token, std::move(cond), std::move(expr));
	}
	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}
	return make_ast<expression_statement>(expr_token, std::move(expr));
}
std::unique_ptr<expression> parser::parse_expression() {
	return parse_binary_expression();
}
std::unique_ptr<expression> parser::parse_binary_expression(int precedence) {
	//token& expr_token = peek(); // not sure what this is for but it was here?
	auto left = parse_unary_expression();
	while (true) {
		token& op = peek();
		if (!is_op(op)) {
			break; // no more operators
		}
		int op_precedence = precedence_of(op.type);
		if (op_precedence < precedence) {
			break; // no more operators with higher precedence
		}

		bool right_associative = is_assignment(op.type);

		advance(); // consume the operator

		// parse the right-hand side expression
		int next_precedence = right_associative ? op_precedence : op_precedence + 1;
		auto right = parse_binary_expression(next_precedence);

		if (is_assignment(op.type)) {
			if (op.type == TT_ASSIGN) {
				left = make_ast<assignment_expression>(op, std::move(left), std::move(right));
			}
			else {
				std::unique_ptr<expression> left_value(static_cast<expression*>(left->clone().release()));
				token_type bin_op = TT_UNKNOWN;
				switch (op.type) {
				case TT_ADD_ASSIGN:      bin_op = TT_ADD; break;
				case TT_SUBTRACT_ASSIGN: bin_op = TT_SUBTRACT; break;
				case TT_MULTIPLY_ASSIGN: bin_op = TT_MULTIPLY; break;
				case TT_DIVIDE_ASSIGN:   bin_op = TT_DIVIDE; break;
				case TT_MODULO_ASSIGN:   bin_op = TT_MODULO; break;
				default: break;
				}
				auto rhs = make_ast<binary_expression>(op, std::move(left_value), std::move(right), bin_op);
				left = make_ast<assignment_expression>(op, std::move(left), std::move(rhs));
			}
		}
		else {
			left = make_ast<binary_expression>(op, std::move(left), std::move(right), op.type);
		}
	}
	return left;
}
std::unique_ptr<expression> parser::parse_unary_expression() {
	token& tok = peek();
	// address-of operator
	if (tok.type == TT_AMPERSAND) {
		advance();
		auto operand = parse_unary_expression();
		return make_ast<address_of_expression>(tok, std::move(operand));
	}
	// deref operator
	if (tok.type == TT_MULTIPLY) {
		advance();
		auto operand = parse_unary_expression();
		return make_ast<deref_expression>(tok, std::move(operand));
	}
	// check for prefix unary operators (steel does not support increment/decrement prefix operators)
	if (tok.type == TT_SUBTRACT || tok.type == TT_NOT) {
		advance();
		auto operand = parse_unary_expression();
		return make_ast<unary_expression>(tok, tok.type, std::move(operand));
	}
	// otherwise, parse primary expression
	return parse_primary_expression();
}
std::unique_ptr<expression> parser::parse_primary_expression() {
	std::unique_ptr<expression> expr = nullptr;

	if (match(TT_IDENTIFIER)) {
		// this is only used for global functions
		// for member/expression based functions,
		// we handle that in the postfix handler
		token& identifier_token = previous();
		// only support type generics for now
		std::vector<type_ptr> generic_args;
		if (ENABLE_GENERICS && match(TT_NOT)) {
			if (!match(TT_LESS)) {
				ERROR_TOKEN(ERR_ANGLE_LEFT_EXPECTED, peek());
				return nullptr;
			}
			// note: we cant use parse_generics here as we want types not parameters
			do {
				type_ptr gen_type = parse_type();
				if (gen_type) {
					generic_args.push_back(gen_type);
				}
				else {
					ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
					return nullptr;
				}
			} while (match(TT_COMMA));
			if (!match(TT_GREATER)) {
				ERROR_TOKEN(ERR_ANGLE_RIGHT_EXPECTED, peek());
				return nullptr;
			}
		}
		if (match(TT_LPAREN)) {
			auto args = parse_expression_list();
			if (!match(TT_RPAREN)) {
				ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
				return nullptr;
			}
			// identifier - function call
			else {
				auto func = make_ast<function_call>(identifier_token, identifier_token.value, std::move(args));
				func->generic_args = generic_args;
				expr = std::move(func);
			}
		}
		else {
			expr = make_ast<identifier_expression>(identifier_token, identifier_token.value);
		}
	}
	else if (match(TT_INTEGER_LITERAL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, DT_I32, literal_token.value);
	}
	else if (match(TT_FLOAT_LITERAL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, DT_FLOAT, literal_token.value);
	}
	else if (match(TT_CHAR_LITERAL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, DT_CHAR, literal_token.value);
	}
	else if (match(TT_STRING_LITERAL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, DT_STRING, literal_token.value);
	}
	else if (match(2, TT_TRUE, TT_FALSE)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, DT_BOOL, literal_token.value);
	}
	else if (match(TT_NULL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, DT_NULL, "null");
		// ^^ generic pointer
	}
	else if (match(TT_THIS)) {
		expr = make_ast<this_expression>(previous());
	}
	else if (match(TT_LPAREN)) {
		expr = parse_expression();
		if (!expr) {
			ERROR_TOKEN(ERR_EXPRESSION_EXPECTED, peek());
			return nullptr;
		}
		if (!match(TT_RPAREN)) {
			ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
			return nullptr;
		}
	}
	else {
		ERROR_TOKEN(ERR_UNEXPECTED_TOKEN_PRIMARY_EXPR, peek());
		return nullptr;
	}

	// handle postfix operators
	while (true) {
		token& tok = peek();
		// resolution expressions
		if (tok.type == TT_ACCESS || tok.type == TT_SCOPE) {
			advance(); // consume '.'/'::'
			if (!match(TT_IDENTIFIER)) {
				ERROR_TOKEN(ERR_MEMBER_NAME_EXPECTED, peek());
				return nullptr;
			}
			token& member_token = previous();
			expr = make_ast<member_expression>(tok, std::move(expr), member_token.value, tok.type);
		}
		// method call
		else if (tok.type == TT_LPAREN) {
			token& prev_tok = previous();
			advance(); // consume '('
			auto args = parse_expression_list();
			if (!match(TT_RPAREN)) {
				ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
				return nullptr;
			}

			// scoped function call
			if (auto* mem = dynamic_cast<member_expression*>(expr.get())) {
				if (mem->access_operator == TT_SCOPE) {
					auto func = make_ast<function_call>(tok, mem->member, std::move(args));
					func->scope = std::move(mem->object);
					expr = std::move(func);
					continue;
				}
			}

			// regular method call
			if (prev_tok.type == TT_IDENTIFIER) {
				expr = make_ast<function_call>(prev_tok, std::move(expr), prev_tok.value, std::move(args));
			}
			else {
				expr = make_ast<function_call>(tok, std::move(expr), std::move(args));
			}
		}
		// index expressions
		else if (tok.type == TT_LBRACKET) {
			advance();
			auto index_expr = parse_expression();
			if (!match(TT_RBRACKET)) {
				ERROR_TOKEN(ERR_RBRACKET_EXPECTED, peek());
				return nullptr;
			}
			expr = make_ast<index_expression>(tok, std::move(expr), std::move(index_expr));
		}
		// increment/decrement operators
		else if (tok.type == TT_INCREMENT || tok.type == TT_DECREMENT) {
			advance();
			expr = make_ast<unary_expression>(tok, tok.type, std::move(expr));
		}
		// as-cast
		else if (tok.type == TT_AS) {
			advance();
			type_ptr cast_type = parse_type();
			if (!cast_type) {
				ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
				return nullptr;
			}
			expr = make_ast<cast_expression>(tok, cast_type, std::move(expr));
		}
		else {
			break;
		}
	}

	return expr;
}
std::unique_ptr<initializer_list> parser::parse_initializer_list() {
	token& init_token = peek();
	if (!match(TT_LBRACE)) {
		ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
		return nullptr;
	}
	auto values = parse_expression_list(TT_RBRACE);
	if (!match(TT_RBRACE)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}
	return make_ast<initializer_list>(init_token, std::move(values));
}
std::unique_ptr<initializer_list> parser::parse_array_initializer() {
	token& init_token = peek();
	if (!match(TT_LBRACKET)) {
		ERROR_TOKEN(ERR_LBRACKET_EXPECTED, peek());
		return nullptr;
	}
	auto values = parse_expression_list(TT_RBRACKET);
	if (!match(TT_RBRACKET)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}
	auto list = make_ast<initializer_list>(init_token, std::move(values));
	list->is_array_initializer = true;
	return list;
}

std::vector<std::unique_ptr<attribute>> parser::parse_attributes() {
	if (!match(TT_HASH)) {
		return {};
	}

	if (!match(TT_LBRACKET)) {
		ERROR_TOKEN(ERR_LBRACKET_EXPECTED, peek());
		return {};
	}

	std::vector<std::unique_ptr<attribute>> attrs;

	do {
		if (!match(TT_IDENTIFIER)) {
			ERROR_TOKEN(ERR_ATTRIBUTE_EXPECTED, peek());
			return {};
		}

		token& attr_name_token = previous();
		attrs.emplace_back(make_ast<attribute>(attr_name_token, attr_name_token.value));
	} while (match(TT_COMMA));

	if (!match(TT_RBRACKET)) {
		ERROR_TOKEN(ERR_RBRACKET_EXPECTED, peek());
		return {};
	}

	return attrs;
}
std::vector<modifier> parser::parse_modifiers() {
	std::vector<modifier> mods;

	while (true) {
		auto& tok = peek();
		if (match(TT_EXTERN)) {
			mods.emplace_back(MOD_EXTERN, tok);
		}
		else if (match(TT_STATIC)) {
			mods.emplace_back(MOD_STATIC, tok);
		}
		else if (match(TT_EXPORT)) {
			mods.emplace_back(MOD_EXPORT, tok);
		}
		else if (match(TT_CONST)) {
			mods.emplace_back(MOD_CONST, tok);
		}
		else break;
	}

	return mods;
}
type_ptr parser::parse_type() {
	// parse through modifiers, e.g. 'const'
	auto type_mods = parse_modifiers();

	// match typename (custom or primitive)
	type_ptr type = data_type::UNKNOWN;
	if (match_primitive() || match(TT_IDENTIFIER)) {
		type = to_data_type(previous());
	}
	else {
		// type mods but no typename
		if (!type_mods.empty()) {
			ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
		}
		// no type or mods, so we can just return null
		return nullptr;
	}
	type->modifiers = type_mods;

	// generic type args (if any)
	if (ENABLE_GENERICS && match(TT_NOT)) {
		if (!match(TT_LESS)) {
			ERROR_TOKEN(ERR_ANGLE_LEFT_EXPECTED, peek());
			return nullptr;
		}
		std::vector<type_ptr> generic_args;
		do {
			type_ptr gen_type = parse_type();
			if (gen_type) {
				generic_args.push_back(gen_type);
			}
			else {
				ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
				return nullptr;
			}
		} while (match(TT_COMMA));
		if (!match(TT_GREATER)) {
			ERROR_TOKEN(ERR_ANGLE_RIGHT_EXPECTED, peek());
			return nullptr;
		}
		type->generic_args = generic_args;
	}

	// parse through any pointer/array modifiers
	while (true) {
		if (match(TT_MULTIPLY)) {
			type = make_pointer(type);
		}
		else if (match(TT_LBRACKET)) {
			auto size_expr = parse_expression();
			if (!match(TT_RBRACKET)) {
				ERROR_TOKEN(ERR_RBRACKET_EXPECTED, peek());
				return nullptr;
			}
			type = make_array(type, std::move(size_expr));
		}
		else {
			break;
		}
	}

	return type;
}
std::vector<std::unique_ptr<expression>> parser::parse_expression_list(token_type end) {
	std::vector<std::unique_ptr<expression>> expressions;
	while (!is_at_end() && !check(end)) {
		auto expr = parse_expression();
		if (expr) {
			expressions.emplace_back(std::move(expr));
		}
		else {
			ERROR_TOKEN(ERR_EXPRESSION_EXPECTED, peek());
			return expressions;
		}
		if (match(TT_COMMA)) {
			continue; // continue to next expression
		}
	}
	return expressions;
}

std::vector<std::unique_ptr<variable_declaration>> parser::parse_parameter_list(token_type end) {
	std::vector<std::unique_ptr<variable_declaration>> parameters;
	if (check(end)) return parameters;
	do {
		auto param = parse_parameter();
		if (param) {
			parameters.emplace_back(std::move(param));
		}
		else {
			ERROR_TOKEN(ERR_PARAMETER_EXPECTED, peek());
			return parameters;
		}
	} while (!is_at_end() && !check(end) && match(TT_COMMA));
	return parameters;
}
std::vector<std::unique_ptr<generic_parameter>> parser::parse_generics() {
	if (!match(TT_LESS)) {
		ERROR_TOKEN(ERR_ANGLE_LEFT_EXPECTED, peek());
		return {};
	}
	std::vector<std::unique_ptr<generic_parameter>> generics;
	while (true) {
		if (match(TT_IDENTIFIER)) {
			token& id_token = previous();
			auto generic = make_ast<generic_parameter>(id_token, id_token.value);
			generics.emplace_back(std::move(generic));

			if (!match(TT_COMMA) && !check(TT_GREATER)) {
				ERROR_TOKEN(ERR_ANGLE_RIGHT_EXPECTED, peek());
				return generics;
			}
		}
		else if (!match(TT_GREATER)) {
			ERROR_TOKEN(ERR_ANGLE_RIGHT_EXPECTED, peek());
			return generics;
		}
		else {
			return generics;
		}
	}
}

token& parser::peek() {
	return _tokens[_position];
}
token& parser::previous() {
	if (_position == 0) {
		return _tokens[0];
	}
	return _tokens[_position - 1];
}
token& parser::consume() {
	return _tokens[_position++];
}
bool parser::match_primitive() {
	if (match(13, TT_I16, TT_I32, TT_I64, TT_U16, TT_U32, TT_U64, TT_FLOAT, TT_DOUBLE, TT_CHAR, TT_STRING, TT_BYTE, TT_BOOL, TT_VOID)) {
		return true;
	}
	return false;
}
bool parser::match(int count, ...) {
	va_list args;
	va_start(args, count);
	for (int i = 0; i < count; i++) {
		token_type type = va_arg(args, token_type);
		if (match(type)) {
			va_end(args);
			return true;
		}
	}
	va_end(args);
	return false;
}
bool parser::match(token_type type) {
	if (is_at_end()) return false;
	if (peek().type != type) return false;
	advance();
	return true;
}
bool parser::check(token_type type) {
	if (is_at_end()) return false;
	return peek().type == type;
}
void parser::advance() {
	if (!is_at_end()) _position++;
}
void parser::synchronize() {
	advance();
	while (!is_at_end()) {
		if (previous().type == TT_SEMICOLON || previous().type == TT_RBRACE) return;

		switch (peek().type) {
		case TT_MODULE:
		case TT_IMPORT:
		case TT_STRUCT:
		case TT_CLASS:
		case TT_INTERFACE:
		case TT_ENUM:
		case TT_FUNC:
		case TT_OVERRIDE:
		case TT_CONST:
		case TT_LET:
		case TT_IF:
		case TT_FOR:
		case TT_WHILE:
		case TT_RETURN:
		case TT_BREAK:
			return;
		default:
			break;
		}
		advance();
	}
}
bool parser::is_at_end() {
	return peek().type == TT_EOF;
}
