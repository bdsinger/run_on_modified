#include <iostream>
#include "FileWatcher.hpp"

int main(int argc, char* argv[]) {

	if (argc < 4) {
		std::cerr << "\n\tUsage: " << argv[0] << " <target_path> <script_path> <poll_interval_ms>" << '\n';
		const char* usage_text = R"END(

	<target_path>:
	Path to the file you want to modify. There must be a path component, even if it is `./some_file` in the current directory.

	<script_path>:
	Path to the command you want to run when the target is modified. If the command is in your `PATH` you don't need to specify the full path, otherwise you do.

	<poll_interval_ms>:
	How long to wait between checking the status of the target's parent directory, in milliseconds.
	)END";
		std::cerr << usage_text << '\n';
		return -1;
	}

	// std::string target = argv[1];
	std::filesystem::path target_path(argv[1]);
	std::string script = argv[2];
	int poll_interval = std::stoi(argv[3]);

	// Create a FileWatcher instance that will check the current folder for changes every N ms for
	// file specified and if modified runs script provided
	std::string dirname = target_path.parent_path();
	std::cout << "Monitoring the directory: " << dirname << "/\n";
	FileWatcher fw{dirname, target_path.filename(), script, std::chrono::milliseconds(poll_interval)};

	// Start monitoring a folder for changes and (in case of changes)
	// run a user provided lambda function
	fw.start([] (std::string file_in_question, std::string target, std::string script, FileStatus status) -> void {
		// Process only regular files, all other file types are ignored		
		if(!std::filesystem::is_regular_file(std::filesystem::path(file_in_question)) && status != FileStatus::erased) {
			return;
		}

		// strip dirname from file_in_question
		std::filesystem::path path_in_question(file_in_question);
		auto candidate = path_in_question.filename();

		// Process only target file
		if (target != candidate) {
			return;
		}

		switch(status) {
			case FileStatus::created:
				std::cout << "File created: " << candidate << '\n';
				break;
			case FileStatus::modified:
				std::cout << "File modified: " << candidate << '\n';
				std::cout << "Running: " << script << '\n';
				system(script.c_str());
				break;
			case FileStatus::erased:
				std::cout << "File erased: " << candidate << '\n';
				break;
			default:
				std::cout << "Error! Unknown file status.\n";
		}
	});
	return 0;
}
