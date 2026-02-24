#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include <mir/mir_function.h>

// mir_module
//
// represents a MIR module containing multiple functions
// 
// note: mir_module does not represent modules in steel
// it is more closely related to an object file or a compilation unit

struct mir_module_meta {
	std::filesystem::path src_relpath; // relative path of the source file that generated this module
};

struct mir_module {
	mir_module() = default;
	mir_module(const mir_module&) = delete;
	mir_module& operator=(const mir_module&) = delete;

	std::string name; // typically the source file name without extension
	std::vector<std::unique_ptr<mir_function>> functions;
	mir_module_meta meta;
};