#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <ast/ast_fwd.h>
#include <representations/entities/entities_fwd.h>

struct module_info;

class module_manager {
public:
	module_manager();

	module_entity* add_module(const std::string& name, module_entity* parent);

	inline bool has_module(const std::vector<std::string>& name_path) const;
	module_entity* get_module(const std::vector<std::string>& name_path);
	module_entity* get_parent(std::shared_ptr<module_entity> module);
	module_entity* get_global_module();

	static std::string module_path_to_full_name(const std::vector<std::string>& name_path);

	std::shared_ptr<function_declaration> entry_point;

private:
	std::unique_ptr<module_entity> _global_module;
	std::unordered_map<std::string, std::unique_ptr<module_entity>> _modules;

	std::shared_ptr<module_info> create_info(const std::string& name);
};