#include "system_linker.h"

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>

#include <sys/host_defs.h>
#include <sys/env.h>

#if defined(STEELC_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace fs = std::filesystem;

namespace {
#if defined(STEELC_PLATFORM_WINDOWS)
    std::vector<std::string> get_pathexts() {
        std::string pathext;
        if (!sys::env::get_var("PATHEXT", pathext)) {
            pathext = ".COM;.EXE;.BAT;.CMD"; // default if env var not set
        }
        std::vector<std::string> out;
        std::stringstream ss(pathext);
        std::string ext;
        while (std::getline(ss, ext, ';')) {
            if (!ext.empty()) {
                // normalize to upper to avoid mismatches
                std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
                out.push_back(ext);
            }
        }
        return out;
    }
#endif

    bool is_executable(const fs::path& p) {
        if (!fs::exists(p) || !fs::is_regular_file(p)) return false;
#if defined(STEELC_PLATFORM_WINDOWS)
        return true;
#else
        std::error_code ec;
        auto perms = fs::status(p, ec).permissions();
        if (ec) return false;
        using fs::perms;
        return (perms & (perms::owner_exec | perms::group_exec | perms::others_exec)) != perms::none;
#endif
    }
}

std::unique_ptr<system_linker> system_linker::probe(const std::vector<std::string>& candidates) {
    const auto& path_entries = sys::env::get_path_entries();
    if (path_entries.empty()) {
        return nullptr;
    }

#if defined(STEELC_PLATFORM_WINDOWS)
    const auto pathexts = get_pathexts();
#endif

    for (const auto& dir : path_entries) {
        for (const std::string& c_filename : candidates) {
            fs::path candidate = fs::path(dir) / c_filename;

            if (is_executable(candidate)) {
                std::string linker_path = fs::canonical(candidate).string();
                return std::unique_ptr<system_linker>(new system_linker(linker_path));
            }

#if defined(STEELC_PLATFORM_WINDOWS)
            if (candidate.extension().empty()) {
                for (const auto& ext : pathexts) {
                    fs::path with_ext = candidate;
                    with_ext.replace_extension(ext);
                    if (is_executable(with_ext)) {
                        std::string linker_path = fs::canonical(with_ext).string();
                        return std::unique_ptr<system_linker>(new system_linker(linker_path));
                    }
                }
            }
#endif
        }
    }
    return nullptr;
}
