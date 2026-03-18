#pragma once

#include <vector>
#include <string>

namespace sys {
	namespace env {
		// safe enviroment variable getter
		bool get_var(const std::string& name, std::string& out);
		// cross-platform PATH entries getter
		std::vector<std::string> get_path_entries();
	}
}