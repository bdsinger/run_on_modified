# run_on_modified

This is simple utility that takes three arguments: what file to watch, what command to run when the file is modified, and how often to check the file for changes. A typical use might be:

    run_on_modified ~/Documents/table.txt ~/Documents/myscript.sh 1000
    
which would monitor the file `table.txt` in your `Documents` folder for changes once per second, and if it changes run the script `myscript.sh` in your Documents folder. The script can be in any language, as long as it is marked executable, or you could run an existing command in your `PATH` you didn't write, like `ls`.

## Background

This is a quick hack to the code from this [Solarian Programmer article][1]. The article demonstrated the addition of [filesystem][2] features to C++ in C++17.

The modifications here are:

 * Running the noexcept version of `std::filesystem::last_write_time()` since otherwise irregular files (like iCloud file stubs or some symlinks) caused uncaught exceptions and an early exit on macOS.
 * Special treatment for modified files, since that is the main application purpose. Other filesystem events remain in the main switch statement for debugging, but could be deleted.
 * The code uses the `contains` method [added to][3] `std::unordered_map` in C++20<sup>1,2</sup>, as suggested by the Solarian Programmer.
 * Disambiguated `path_to_watch` in the lambda action function from `path_to_watch` in the main class constructor. Renamed the lambda version `file_in_question`
 
## Requirements
 
A recent version of [clang][4] (2019 or newer) to use the `Makefile` as-is. More generally, a compiler supporting C++17 and draft C++20. To use `gcc`, edit the Makefile's `CXX` and possibly `CXXFLAGS` lines.
 
## Building
 
Just type `make`

## Installing

Put the binary `run_on_modified` in your `PATH`, or run it via `/path/to/run_on_modified`

## Usage

`run_on_modified` takes 3 required arguments in a required order:

    run_on_modified <target_path> <script_path> <poll_interval_ms>
 
    <target_path>:
    Path to the file you want to modify. There must be a path component, even if it is `./some_file` in the current directory.
 
    <script_path>:
    Path to the command you want to run when the target is modified. If the command is in your `PATH` you don't need to specify the full path, otherwise you do.
    
    <poll_interval_ms>:
    How long to wait between checking the status of the target's parent directory, in milliseconds.
   
## Example

    git clone https://github.com/bdsinger/run_on_modified.git
    cd run_on_modified
    chmod +x ./example_to_run.sh
    make
    ./run_on_modified ./example_to_modify.txt ./example_to_run.sh 1000

In another window:

    touch ./example_to_modify.txt

In the first window you should see the output:

    Monitoring the directory: .
    File modified: "example_to_modify.txt"
    Running: ./example_to_run.sh
    target modified 

## Suggested improvements

The code could be made much more efficient. It keeps track of the modification times of all files in a directory, recursively, which could be lots of files when you only care about one file! The code originates from an example where any changes to the file hierarchy are detected and reported, including deletion and creation.

## Warning: Insecure and untested

The code is insecure. The command to run is executed via `system()` without inspection. The input arguments are not validated. The code has not been well exercised. Though its scope is limited, there could be serious bugs. Use at your own risk.

---
<sup>1</sup> `clang++ -std=c++2a ...` as of June 2021 (clang `12.0.0`)<br>
<sup>2</sup>  The [article][1] provides a `FileWatcher::contains` method if you'd rather not use any C++20-only features.

[1]: https://solarianprogrammer.com/2019/01/13/cpp-17-filesystem-write-file-watcher-monitor/ "Article text for \"C++17 Filesystem - Writing a simple file watcher\""
[2]: https://en.cppreference.com/w/cpp/header/filesystem "C++17 reference documentation for std::filesystem"
[3]: https://en.cppreference.com/w/cpp/container/unordered_map/contains "C++20 reference documentation for std::unordered_map::contains"
[4]: https://clang.llvm.org "Home of the Clang C/C++ Compiler, part of the LLVM Project"

