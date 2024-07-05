# BlueBrain HPC Team C++ Development Guidelines

This document describes both C++ development guidelines adopted by
HPC team, and the tools and processes required to
ensure they are properly followed over time.

## Contributing

Tristan CAREL (tristan0x) is in charge of this repository activity and well-being but every change is welcome through pull-request.

Should you want to contribute to the naming conventions,
please refer to the dedicated [contributing document](./formatting/CONTRIBUTING.md) first.

## Documentation

Development Guidelines are split in the following sections:
* [Tooling](./Tooling.md)
* [Development Lifecycle](./DevelopmentLifecycle.md)
* [Code Formatting](./formatting/README.md)
* [Naming Conventions](./NamingConventions.md)
* [Code Documentation](./Documentation.md)
* Best Practices
* Python bindings

## Status

This project in currently under development, and shall not provide the features
its documentation pretends. Here is a raw summary of the status:

| Feature               | Definition         | Documentation      | Integration         | Adoption (>10 proj) |
| --------------------- | ------------------ | ------------------ | ------------------  | ------------------- |
| ClangFormat           | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark:  | WIP                 |
| ClangTidy             | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark:  | WIP                 |
| Naming Conventions    | :heavy_check_mark: | :heavy_check_mark: | :no_entry_sign: N/A | :no_entry_sign: N/A |
| Writing Documentation | :heavy_check_mark: | :heavy_check_mark: |                     |                     |
| Project template      | WIP                |                    |                     |                     |
| Good Practices        |                    |                    |                     |                     |
| Memory Check          |                    |                    |                     |                     |
| UT Code Coverage      |                    |                    |                     |                     |

## CMake Project

This directory provides a CMake project that allows one to use the tools and the processes
described in this document. This project requires CMake version 3.6 or higher.

### Requirements

This CMake project expects for the following utilities to be available:
* [Python 3.5 or higher](https://python.org)
* [ClangFormat 7](https://releases.llvm.org/7.0.0/tools/clang/docs/ClangFormat.html)
* [ClangTidy 7](https://releases.llvm.org/7.0.0/tools/clang/tools/extra/docs/clang-tidy/index.html)
* [cmake-format](https://github.com/cheshirekow/cmake_format) Python package
* [pre-commit](https://pre-commit.com/) Python package

Optionally, it will also look for:
* [valgrind](http://valgrind.org/) memory checker
* code coverage utilities like gcov, lcov, or gcov

### Installation

You can import this CMake project into your Git repository using a git submodule:
```
git submodule add https://github.com/BlueBrain/hpc-coding-conventions.git
git submodule update --init --recursive
```

Note: if your project already has a `cmake` sub-directory, it is recommended to create the
submodule in this directory instead of top-level.

Then simply add the following line in the top `CMakeLists.txt`, after your project
declaration:
```
project(mylib CXX)
# [...]
add_subdirectory(hpc-coding-conventions/cpp)
```

After cloning or updating this git submodule, run CMake to take benefits of the latest changes.
This will setup or update git [pre-commit](https://pre-commit.com) hooks of this repository.

### Usage

#### Code Formatting

To activate code formatting of both C/C++ and CMake files,
enable CMake variable `${PROJECT}_FORMATTING` where `${PROJECT}` is the name given
to the CMake `project` function.

For instance, given a project `foo`:

`cmake -Dfoo_FORMATTING:BOOL=ON <path>`

To individually enable the formatting of C/C++ code or CMake files, use the following
CMake variables:

* `${PROJECT}_CLANG_FORMAT:BOOL`
* `${PROJECT}_CMAKE_FORMAT:BOOL`

##### Usage

This will add the following *make* targets:

* `clang-format`: to format C/C++ code.
* `check-clang-format`: the target fails if at least one C/C++ file has improper format.
* `cmake-format`: to format CMake files.
* `check-cmake-format`: the target fails it at least one CMakef file has improper format.

##### Advanced configuration

A list of CMake cache variables can be used to customize the code formatting:

* `${PROJECT}_ClangFormat_OPTIONS`: additional options given to `clang-format` command.
  Default value is `""`.
* `${PROJECT}_ClangFormat_FILES_RE`: list of regular expressions matching C/C++ filenames
  to format. Default is:<br/>
  `"^.*\\\\.c$$" "^.*\\\\.h$$" "^.*\\\\.cpp$$" "^.*\\\\.hpp$$"`
* `${PROJECT}_ClangFormat_EXCLUDES_RE`: list of regular expressions to exclude C/C++ files
  from formatting. Default value is:<br/>
  `".*/third[-_]parties/.*$$" ".*/third[-_]party/.*$$"`

  Regular expressions are tested against the **full file path**.
* `${PROJECT}_ClangFormat_DEPENDENCIES`: list of CMake targets to build before
  formatting C/C++ code. Default value is `""`

Unlike `${PROJECT}_FORMATTING` which is supposed to be defined by the user,
These variables are already defined and may be overridden inside your CMake project.
They are CMake _CACHE_ variables whose value must be forced.
For instance, to ignore code of third-parties located in `ext/` subdirectory
(`third[-_]part(y|ies)` regular expression by default), add this to your CMake project:

```cmake
set(
  foo_ClangFormat_EXCLUDES_RE "${PROJECT_SOURCE_DIR}/ext/.*$$"
  CACHE STRING "list of regular expressions to exclude C/C++ files from formatting"
  FORCE)
```

##### Continuous Integration

Define `${PROJECT}_TEST_FORMATTING:BOOL` CMake variable to enforce formatting during
the `test` make target.

#### Static Analysis

To activate static analysis of C++ files with clang-tidy, enable
the CMake variable `${PROJECT}_STATIC_ANALYSIS` where `${PROJECT}` is the name
given to the CMake `project` function.

For instance, given a project `foo`:

`cmake -DFoo_STATIC_ANALYSIS:BOOL=ON <path>`

##### Usage

This will provide a `clang-tidy` *make* target that will perform static analysis
of all C++ files. Target fails as soon as one defect is detected among the files.

It will also activate static analysis report during the compilation phase.

##### Advanced configuration

A list of CMake cache variables can be used to customize static analysis:

* `${PROJECT}_ClangTidy_OPTIONS`: additional options given to `clang-tidy` command.
  Default value is `""`.
* `${PROJECT}_ClangTidy_FILES_RE`: list of regular expressions matching C/C++ filenames
  to check. Default is:<br/>
  `"^.*\\\\.c$$" "^.*\\\\.h$$" "^.*\\\\.cpp$$" "^.*\\\\.hpp$$"`
* `${PROJECT}_ClangTidy_EXCLUDES_RE`: list of regular expressions to exclude C/C++ files
  from static analysis. Default value is:<br/>
  `".*/third[-_]parties/.*$$" ".*/third[-_]party/.*$$"`
* `${PROJECT}_ClangTidy_DEPENDENCIES`: list of CMake targets to build before
  check C/C++ code. Default value is `""`

These variables are meant to be overridden inside your CMake project.
They are CMake _CACHE_ variables whose value must be forced.

##### Continuous Integration

Define `${PROJECT}_TEST_STATIC_ANALYSIS:BOOL` CMake variable to enforce formatting during
the `test` make target.

#### Pre-Commit

Enable CMake variable `${PROJECT}_PRECOMMIT` to enable automatic checks
before git commits.

For instance, given a project `foo`:

`cmake -Dfoo_PRECOMMIT:BOOL=ON <path>`

if `${PROJECT}_FORMATTING` CMake variable is enabled, when performing a git
commit, a succession of checks will be executed to ensure that your change
complies with the coding conventions. It will be discarded otherwise.
