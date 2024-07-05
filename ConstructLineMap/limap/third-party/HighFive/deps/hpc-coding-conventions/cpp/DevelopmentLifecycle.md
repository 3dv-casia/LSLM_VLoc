# Development Lifecycle

## Writing a contribution

### Checks before submission

C++ developers should ensure that their contributions:
* are unit-tested and unit-tests pass
* pass static analysis checks
* follow naming conventions
* follow documentation conventions
* follow formatting conventions

To save both time and effort, they can:
* Configure their favorite IDE so that it complies with the formatting
  conventions of the project.
* Use dedicated *make* targets to easily run tests, format code, and run
  code static analyzer.
* Use git precommit hooks to prevent committing unformatted code.

## Submitting a contribution

### Continuous Integration

Continuous integration may control most of the checks described above.
It can also perform additional checks that should fail if a new error is spotted:
* execute Valgrind memory checker.
* compute code coverage of unit-tests and/or documentation snippets and reject
  the merge request if it decreased or is below a certain value..

### Code Review

A code change cannot be accepted unless it has been approved by one or several
core maintainers of the project the change is contributing to.

A reviewer may ensure that the contribution:
* is well designed and efficient
* is understandable and documented enough
* follows the naming conventions of the project
