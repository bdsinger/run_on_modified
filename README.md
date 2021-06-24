# run_on_modified

This is a quick hack to the code from this [Solarian Programmer article][1]. The article demonstrated the addition of [filesystem][2] features to C++ in C++17.

The modifications here are:

 * Running the noexcept version of `std::filesystem::last_write_time()` since otherwise irregular files (like iCloud file stubs or some symlinks) caused uncaught exceptions and an early exit on macOS.
 * Special treatment for modified files, since that is the main application purpose. Other filesystem events remain in the main switch statement for debugging, but could be deleted.
 * The code uses the `contains` method [added to][3] `std::unordered_map` in C++20<sup>1,2</sup>, as suggested by the Solarian Programmer.
 * Disambiguated `path_to_watch` in the lambda action function from `path_to_watch` in the main class constructor. Renamed the lambda version `file_in_question`
 * Broke paths into parent directory (ie dirname) component and filename (ie basename) component, so that user needn't pass in "./filename" if watching pwd.

---
<sup>1</sup> `clang++ -std=c++2a ...` as of June 2021 (clang `12.0.0`)<br>
<sup>2</sup>  The [article][1] provides a `FileWatcher::contains` method if you'd rather not use any C++20-only features.

[1]: https://solarianprogrammer.com/2019/01/13/cpp-17-filesystem-write-file-watcher-monitor/ "Article text for \"C++17 Filesystem - Writing a simple file watcher\""
[2]: https://en.cppreference.com/w/cpp/header/filesystem "C++17 reference documentation for std::filesystem"
[3]: https://en.cppreference.com/w/cpp/container/unordered_map/contains "C++20 reference documentation for std::unordered_map::contains"

