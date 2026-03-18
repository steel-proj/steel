#include "data_type.h"

#include <memory>
#include <string>

#include <representations/types/custom_type.h>
#include <representations/types/container_types.h>
#include <representations/types/enum_type.h>
#include <representations/types/function_type.h>
#include <representations/types/type_utils.h>

data_type* data_type::get(data_type_kind primitive) {
	return std::make_shared<data_type>(primitive);
}

data_type* data_type::UNKNOWN = std::make_unique<data_type>(DT_UNKNOWN);

bool data_type::operator==(const data_type& other) const {
	if (primitive != other.primitive) {
		return false;
	}
	// TODO: account for generics + mods
	return true;
}
bool data_type::operator!=(const data_type& other) const {
	return !(*this == other);
}

custom_type* data_type::as_custom() {
	if (is_custom()) {
		return static_cast<custom_type*>(this);
	}
	return nullptr;
}
array_type* data_type::as_array() {
	if (is_array()) {
		return static_cast<array_type*>(this);
	}
	return nullptr;
}
pointer_type* data_type::as_pointer() {
	if (is_pointer()) {
		return static_cast<pointer_type*>(this);
	}
	return nullptr;
}
data_type* data_type::as_reference() {
	// TEMPORARY
	return nullptr;
}
enum_type* data_type::as_enum() {
	if (is_enum()) {
		return static_cast<enum_type*>(this);
	}
	return nullptr;	
}
generic_type* data_type::as_generic() {
	if (is_generic()) {
		return static_cast<generic_type*>(this);
	}
	return nullptr;
}
function_type* data_type::as_function() {
	if (is_function()) {
		return static_cast<function_type*>(this);
	}
	return nullptr;
}

bool data_type::is_primitive() const {
	return true;
}
bool data_type::is_indexable() const {
	if (primitive == DT_STRING) {
		return true;
	}
	return false;
}

int data_type::size_of() const {
	return size_of_primitive(primitive);
}

std::string data_type::name() const {
	if (primitive == DT_UNKNOWN) {
		return "<Unknown Type>";
	}
	return to_string(primitive);
}

std::shared_ptr<data_type> data_type::clone() const {
	return std::make_shared<data_type>(*this);
}
