#include <string>
#include <iostream>

#include <output/output.h>
#include <output/router.h>
#include <output/sinks/stream_sink.h>
#include <output/sinks/file_sink.h>
#include <diagnostics/diagnostics.h>
#include <input/console_args.h>
#include <input/commands/steelc_command_manager.h>
#include <input/commands/steelc_command.h>

static int steelc_main(int argc, char** argv) {
	console_args args(argc, argv);

	if (args.count() < 2) {
		diagnostics::error("Usage: steelc <command> [<args>]\n");
		return 1;
	}
	std::string command = *args.get_arg(1); // safe deref

	steelc_command_manager cmd_manager;
	auto cmd = cmd_manager.get_command(command);
	if (!cmd) {
		diagnostics::error("Error: unknown command: {}\n", command);
		return 1;
	}

	bool command_success = false;
	try {
		command_success = cmd->execute(args.slice(2));
	}
	catch (const std::exception& e) {
		diagnostics::error("Error: fatal exception while executing command '{}': {}\n", command, e.what());
		command_success = false;
	}

	return command_success ? 0 : 1;
}

int main(int argc, char** argv) {
	try {
		return steelc_main(argc, argv);
	}
	catch (const std::exception& e) {
		diagnostics::error("Error: fatal exception executing steelc_main: {}", e.what());
	}
	catch (...) {
		diagnostics::error("Error: unknown fatal exception executing steelc_main");
	}
}