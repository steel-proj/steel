#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <llvm/IR/Module.h>

#include <codegen/code_artifact.h>
#include <sys/target_triple.h>

// llvm_native_writer
//
// utility class responsible for converting llvm modules to
// native binary/text data (e.g., object files, native assembly, etc.)

class llvm_native_writer {
public:
	llvm_native_writer(llvm::Module* module, const target_triple& target, const std::string& cpu)
		: _module(module), _target(target), _cpu(cpu) {
	}

	bool write_object(std::vector<uint8_t>& output_buffer);

	inline std::string get_error() const {
		return error;
	}

private:
	llvm::Module* _module;
	target_triple _target;
	std::string _cpu;

	std::string error;
};