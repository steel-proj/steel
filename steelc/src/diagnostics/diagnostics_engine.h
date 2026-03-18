#pragma once

#include <vector>
#include <ranges>
#include <unordered_map>

#include <diagnostics/compilation_diagnostic.h>
#include <error/internal.h>

class diagnostics_engine {
public:
	inline size_t error_count() {
		return filter_diagnostics(compilation_diagnostic_kind::ERROR).size();
	}
	inline size_t error_count() const {
		return filter_diagnostics(compilation_diagnostic_kind::ERROR).size();
	}

	inline void add_diagnostic(const compilation_diagnostic& diag) {
		diagnostics.push_back(diag);
		filtered_cache.clear();
	}

	inline const std::vector<compilation_diagnostic>& get_diagnostics() const {
		return diagnostics;
	}
	inline const std::vector<compilation_diagnostic>& get_diagnostics(compilation_diagnostic_kind filter) {
		return filter_diagnostics(filter);
	}
	inline const std::vector<compilation_diagnostic>& get_diagnostics(compilation_diagnostic_kind filter) const {
		return filter_diagnostics(filter);
	}

private:
	std::vector<compilation_diagnostic> diagnostics;
	std::unordered_map<compilation_diagnostic_kind, std::vector<compilation_diagnostic>> filtered_cache;

	inline std::vector<compilation_diagnostic> filter_diagnostics(compilation_diagnostic_kind kind) {
		if (filtered_cache.contains(kind)) {
			return filtered_cache.at(kind);
		}

		std::vector<compilation_diagnostic> result;
		for (const auto& diag : diagnostics) {
			if (diag.kind == kind) {
				result.push_back(diag);
			}
		}
		filtered_cache[kind] = result;
		return result;
	}
	inline std::vector<compilation_diagnostic> filter_diagnostics(compilation_diagnostic_kind kind) const {
		if (filtered_cache.contains(kind)) {
			return filtered_cache.at(kind);
		}

		std::vector<compilation_diagnostic> result;
		for (const auto& diag : diagnostics) {
			if (diag.kind == kind) {
				result.push_back(diag);
			}
		}
		return result;
	}
};
