# C++ Code Formatting Documentation Generation Utility

This directory provides both documentation chunks and scripts to build
an HTML document describing C++ code formatting conventions adopted by
the BlueBrain HPC team.

## Requirements

* A Python environment with *pyyaml* and *jinja* packages installed. A
`Pipfile` is provided at repository top-level as a courtesy to setup
such environment with *pipenv*.
* ClangFormat 7
* Pandoc to generate HTML document (optional)

## How to build the documentation

Execute command `make` to build both `README.html` and `README.md`

## How to edit the documentation?

Use GitHub pull-request. Make sure the HTML documentation (or at least the
Markdown one) builds properly.

Generated `README.md` is part of the repository. Make sure to rebuild it
and to include the changes in your pull-request.

Edit `formatting.md.jinja` template or C++ code snippets in the
[`snippets`](./snippets) directory. A C++ snippet has the following structure:

```cpp
// TITLE

// optional *markown* description
// Multiline description is supported
// clang-format: KEY
// clang-format: KEY: VALUE


template <typename T> your cpp(code);
```

* TITLE: used as section in generated documentation
* optional description is included before the code snippet
* If a C++ snippet highlights a particular clang-format configuration
  key, then it should be named after it.
* Additional Clang-Format key/value can be specified in description.
  If value is not specified, then it is retrieved from clang-format
  configuration.
