#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

#include <lexer/token.h>
#include <parser/modifier.h>
#include <utils/iclonable.h>
#include <representations/types/types_fwd.h>

enum data_type_kind : uint32_t {
	DT_UNKNOWN,
	DT_CUSTOM,
	DT_ENUM,

	DT_I16,
	DT_I32,
	DT_I64,
	DT_U16,
	DT_U32,
	DT_U64,
	DT_FLOAT,
	DT_DOUBLE,
	DT_CHAR,
	DT_WIDECHAR,
	DT_STRING,
	DT_WIDESTRING,
	DT_BYTE,
	DT_BOOL,
	DT_VOID,
	DT_NULL,

	DT_ARRAY,
	DT_POINTER,
	DT_REFERENCE,

	// special type for functions
	DT_FUNCTION,

	// special type for generic type params
	DT_GENERIC,
};

class data_type {
public:
	static data_type* UNKNOWN;
	static data_type* NULL_TYPE;

	virtual bool operator==(const data_type& other) const;
	bool operator!=(const data_type& other) const;

	inline bool is_unknown() const {
		return _primitive == DT_UNKNOWN;
	}
	inline bool is_void() const {
		return _primitive == DT_VOID;
	}
	// i16, i32, i64 (unsigned types are not supported yet)
	inline bool is_integer() const {
		return _primitive == DT_I16 || _primitive == DT_I32 || _primitive == DT_I64
			|| _primitive == DT_U16 || _primitive == DT_U32 || _primitive == DT_U64;
	}
	// integer, enum, char, bool
	inline bool is_integral() const {
		return is_integer() || is_enum() || is_character() || is_bool();
	}
	// float, double
	inline bool is_floating_point() const {
		return _primitive == DT_FLOAT || _primitive == DT_DOUBLE;
	}
	// integer or floating point (NOT integral)
	inline bool is_numeric() const {
		return is_integer() || is_floating_point();
	}
	// char, wchar
	inline bool is_character() const {
		return _primitive == DT_CHAR || _primitive == DT_WIDECHAR;
	}
	// string, wstring
	inline bool is_text() const {
		return _primitive == DT_STRING || _primitive == DT_WIDESTRING;
	}
	inline bool is_bool() const {
		return _primitive == DT_BOOL;
	}
	inline bool is_null() const {
		return _primitive == DT_NULL;
	}

	inline bool is_custom() const {
		return _primitive == DT_CUSTOM;
	}
	inline bool is_array() const {
		return _primitive == DT_ARRAY;
	}
	inline bool is_pointer() const {
		return _primitive == DT_POINTER;
	}
	inline bool is_reference() const {
		return _primitive == DT_REFERENCE;
	}
	inline bool is_enum() const {
		return _primitive == DT_ENUM;
	}
	inline bool is_generic() const {
		// IMPORTANT!
		// generic types are used to represent generic parameter usages
		// NOT types with generic arguments
		// to check if a type is a generic type instantiation you should
		// use is_generic_instance()
		return _primitive == DT_GENERIC;
	}
	inline bool is_generic_instance() const {
		return !generic_args.empty();
	}
	inline bool is_function() const {
		return _primitive == DT_FUNCTION;
	}

	inline bool is_valid_object_type() const {
		return is_primitive()
			|| is_custom()
			|| is_array()
			|| is_pointer()
			|| is_enum();
	}

	custom_type* as_custom();
	array_type* as_array();
	pointer_type* as_pointer();
	data_type* as_reference();
	enum_type* as_enum();
	generic_type* as_generic();
	function_type* as_function();

	virtual bool is_primitive() const;
	virtual bool is_indexable() const;

	virtual int size_of() const;

	virtual std::string name() const;

	std::vector<modifier> modifiers;
	std::vector<data_type*> generic_args;
	code_span span;

protected:
	data_type(data_type_kind primitive)
		: _primitive(primitive) {
	}

private:
	friend class type_factory;

	data_type_kind _primitive;
};
