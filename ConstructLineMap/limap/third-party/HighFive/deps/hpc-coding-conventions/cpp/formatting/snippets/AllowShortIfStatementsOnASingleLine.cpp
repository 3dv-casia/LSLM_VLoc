// Conditions

// Break even for simple `if` statement and always put body inside braces.
//
// See clang-tidy check
// [readability-braces-around-statements](http://releases.llvm.org/7.0.0/tools/clang/tools/extra/docs/clang-tidy/checks/readability-braces-around-statements.html#readability-braces-around-statements)

if (a) {
    return;
}
