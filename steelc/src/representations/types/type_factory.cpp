#include "type_factory.h"

#include <memory>
#include <vector>

#include <representations/types/data_type.h>

data_type* type_factory::get_primitive(data_type_kind primitive) {
	// check if type already exists
	for (const auto& type : _types) {
		if (type->primitive == primitive) {
			return type.get();
		}
	}

	// create
	auto new_type = std::make_unique<data_type>(primitive);
	data_type* new_type_ptr = new_type.get();
	_types.push_back(std::move(new_type));
	return new_type_ptr;
}
