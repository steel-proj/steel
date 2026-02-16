#include "mir_instr.h"

#include <unordered_set>

bool is_terminator(const mir_instr& instr) {
    switch (instr.kind) {
    case mir_instr_opcode::RET:
    case mir_instr_opcode::BRA:
    case mir_instr_opcode::BRA_CND:
        return true;
    default:
        return false;
    }
}
