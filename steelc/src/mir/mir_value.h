#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <mir/mir_type.h>

// mir_value
//
// represents a value in MIR such as a variable or temporary

class mir_value {
public:
	using id_type = int64_t;

public:
	mir_value() = default;
	explicit mir_value(const mir_type& ty)
		: id(next_value_id++), type(ty) {
	}
	mir_value(const mir_type& ty, const std::string& name)
		: id(next_value_id++), type(ty) {
		assign_name(name);
	}

	inline bool valid() const {
		return id != -1;
	}

	inline id_type get_id() const {
		return id;
	}
	inline mir_type get_type() const {
		return type;
	}

	// for debugging and readability purposes
	void assign_name(const std::string& name) const;
	std::string get_name() const;

private:
	static id_type next_value_id;
	static std::unordered_map<id_type, std::string> value_names;

	id_type id = -1;
	mir_type type{};
};
