#pragma once
#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

// Define available file changes
enum class FileStatus {created, modified, erased};
 
class FileWatcher {
	public:
		std::error_code last_ec;
		std::string path_to_watch;
		std::string target;
		std::string script;

		// Time interval at which we check the base folder for changes
		std::chrono::duration<int, std::milli> delay;

		// Keep a record of files from the base directory and their last modification time
		FileWatcher(std::string path_to_watch, std::string target, std::string script,  std::chrono::duration<int, std::milli> delay) : path_to_watch{path_to_watch}, target{target}, script{script}, delay{delay} {
			for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
				paths_[file.path().string()] = std::filesystem::last_write_time(file, last_ec);
			}
		}

		// Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
		void start(const std::function<void (std::string, std::string, std::string, FileStatus)> &action) {
			while(running_) {
				// Wait for "delay" milliseconds
				std::this_thread::sleep_for(delay);

				// Check if a file was erased
				auto it = paths_.begin();
				while (it != paths_.end()) {
					if (!std::filesystem::exists(it->first)) {
						 action(it->first, target, script, FileStatus::erased);
						 it = paths_.erase(it);
					}
					else {
						 it++;
					}
				}

				// Check if a file was created or modified
				for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
					auto current_file_last_write_time = std::filesystem::last_write_time(file);

					// Created
					if(!paths_.contains(file.path())) {
						 paths_[file.path().string()] = current_file_last_write_time;
						 action(file.path().string(), target, script, FileStatus::created);
					// Modified
					} else {
						if(paths_[file.path().string()] != current_file_last_write_time) {
							paths_[file.path().string()] = current_file_last_write_time;
							action(file.path().string(), target, script, FileStatus::modified);
						}
					}
				}
			}
		}
	private:
		std::unordered_map<std::string, std::filesystem::file_time_type> paths_;
		bool running_ = true;
};