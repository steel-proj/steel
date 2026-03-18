#pragma once

#include <memory>
#include <vector>

#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>

class type_factory {
public:
	data_type* get_primitive(data_type_kind primitive);

private:
	std::vector<std::unique_ptr<data_type>> _types;
};
