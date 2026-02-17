#include <string>
#include <vector>

// name_path
//
// represents a path of names, used for scoped names like functions,
// types, and variables. at least 'name' is guaranteed to be present,
// and 'scopes' may be empty if the name is not scoped.

class name_path {
public:
	name_path() = default;
	name_path(const std::string& name)
		: name(name) {
	}
	name_path(const std::vector<std::string>& scopes, const std::string& name)
		: scopes(scopes), name(name) {
	}

	std::string flatten(const std::string& sep = "::") const;

	std::vector<std::string> scopes; // outer to inner
	std::string name; // unqualified name
};