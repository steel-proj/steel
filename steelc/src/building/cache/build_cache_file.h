#pragma once

#include <filesystem>
#include <cstdint>
#include <unordered_map>
#include <string>

#include <formatting/formatting.h>
#include <building/cache/source_metadata.h>
#include <building/cache/artifact_metadata.h>
#include <steelc/steelc_definitions.h>

class build_cache_file {
public:
	static constexpr auto EXTENSION = ".dat";
	static constexpr auto MAGIC = "SCBC";
	static constexpr int VERSION_LATEST = 3;

public:
	build_cache_file() = default;

	bool deserialize(const std::filesystem::path& path);
	bool serialize(const std::filesystem::path& path) const;

	inline bool outdated() const {
		return version != VERSION_LATEST || steelc_version != STEELC_VERSION_PACKED_U32;
	}
	void upgrade();

	inline std::unordered_map<std::filesystem::path, source_metadata>& get_src_metadata() {
		return src_metadata;
	}
	inline void set_src_metadata(const std::filesystem::path& path, const source_metadata& meta) {
		src_metadata[path] = meta;
	}
	inline void remove_src_metadata(const std::filesystem::path& path) {
		src_metadata.erase(path);
	}

	// should maybe be const idk?
	inline std::unordered_map<std::filesystem::path, artifact_metadata>& get_artifact_metadata() {
		return art_metadata;
	}
	inline void set_artifact_metadata(const std::filesystem::path& path, const artifact_metadata& meta) {
		art_metadata[path] = meta;
	}
	inline void remove_artifact_metadata(const std::filesystem::path& path) {
		art_metadata.erase(path);
	}

private:
	std::unordered_map<std::filesystem::path, source_metadata> src_metadata;
	std::unordered_map<std::filesystem::path, artifact_metadata> art_metadata;
	int version = VERSION_LATEST;
	uint32_t steelc_version = STEELC_VERSION_PACKED_U32;

	void write_string(std::ofstream& file, const std::string& str) const;
	void read_string(std::ifstream& file, std::string* str) const;
	void write_path(std::ofstream& file, const std::filesystem::path& path) const;
	void read_path(std::ifstream& file, std::filesystem::path* path) const;
};