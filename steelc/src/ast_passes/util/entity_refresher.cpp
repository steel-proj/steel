#include "entity_refresher.h"

#include <memory>

#include <ast/ast.h>

void entity_refresher::visit(identifier_expression& expr) {
	expr.entity_ref.resolve(sym_table);
}
void entity_refresher::visit(member_expression& expr) {
	expr.entity_ref.resolve(sym_table);
}
