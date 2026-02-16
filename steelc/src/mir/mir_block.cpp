#include "mir_block.h"

const mir_instr* mir_block::get_terminator() const {
	if (instructions.empty()) {
		return nullptr;
	}
	const mir_instr& last_instr = instructions.back();
	if (is_terminator(last_instr)) {
		return &last_instr;
	}
	return nullptr;
}
