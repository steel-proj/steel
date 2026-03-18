#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include <lexer/token.h>
#include <lexer/token_type.h>
#include <representations/types/types_fwd.h>

enum data_type_kind : uint32_t;

enum data_type_kind to_primitive(token_type tt);
const type_ptr to_data_type(token& tk);
const type_ptr to_data_type(token_type tt);
const type_ptr to_data_type(data_type_kind pt);
const type_ptr to_data_type(const std::string& type_name);
