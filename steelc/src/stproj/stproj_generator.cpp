#include "stproj_generator.h"

#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>

#include <diagnostics/diagnostics.h>
#include <output/output.h>

bool stproj_generator::generate_new_project(const std::string& name, const std::filesystem::path& location) {
	const auto project_dir = location / name;
	if (std::filesystem::exists(project_dir)) {
		diagnostics::error("Error: A project or directory with the same name already exists at: \"{}\"", project_dir);
		return false;
	}

	std::filesystem::create_directories(project_dir);
	if (!std::filesystem::exists(project_dir) || !std::filesystem::is_directory(project_dir)) {
		diagnostics::error("Error: Failed to create project directory at \"{}\".\n", project_dir);
		return false;
	}

	const auto project_file_path = project_dir / (name + ".stproj");
	if (!create_project_file(name, project_file_path)) {
		diagnostics::error("Error: Failed to create project file at \"{}\".\n", project_file_path);
		return false;
	}

	const auto src_dir = project_dir / "src";
	if (!std::filesystem::create_directory(src_dir)) {
		diagnostics::error("Error: Failed to create source directory at \"{}\".\n", src_dir);
		return false;
	}

	if (!create_main_file(src_dir)) {
		diagnostics::error("Error: Failed to create main source file at \"{}\".\n", src_dir / "main.st");
		return false;
	}

	output::print("Successfully created project \"{}\" at: {}\n", name, project_dir.string());
	return true;
}

bool stproj_generator::create_project_file(const std::string& name, const std::filesystem::path& path) {
	std::ofstream project_file(path);
	if (!project_file.is_open()) {
		return false;
	}

	project_file << "name = \"" << name << "\"\n";
	project_file << "version = \"0.1.0\"\n";
	project_file << "type = \"executable\"\n\n";

	project_file << "sources = [ \"src/main.st\" ]";

	return true;
}
bool stproj_generator::create_main_file(const std::filesystem::path& src_dir) {
	const auto main_file_path = src_dir / "main.st";
	std::ofstream main_file(main_file_path);
	if (!main_file.is_open()) {
		return false;
	}

	main_file << "func main() -> int {\n";
	main_file << "\t// TODO: Implement your program logic here\n";
	main_file << "\treturn 0;\n";
	main_file << "}\n";

	return true;
}
