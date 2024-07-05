# BlueBrain HPC Team C++ Code Formatting

This document provides a set of examples to highlight C++
code formatting conventions adopted by BlueBrain HPC team.

# Similar work

* NSE Team also provides [such document](https://bbpteam.epfl.ch/project/spaces/pages/viewpage.action?spaceKey=BBPNSE&title=Code+style)

# Conventions


## Access Modifiers Indentation inside `struct` and `class`
access modifiers may be indented this way:

### Example
```cpp
class Foo {
  public:
    Foo();

  private:
    int data;
};

```
### Clang-Format configuration
* `AccessModifierOffset: -2`

## Horizontally align arguments after an open bracket

### Example
```cpp
thisIsAVeryLongMethodName(thisIsTheFirstArgument,
                          andHereIsTheSecondArgument,
                          theThirdArgument,
                          theFourthArgument,
                          theFifthArgument);

```
### Clang-Format configuration
* `AlignAfterOpenBracket: Align`

## Do not align consecutive assignments

### Example
```cpp
int aaaa = 12;
int b = 23;
int ccc = 23;

```
### Clang-Format configuration
* `AlignConsecutiveAssignments: False`

## Do not align consecutive declarations

### Example
```cpp
int aaaa = 12;
float b = 23;
std::string ccc = 23;

```
### Clang-Format configuration
* `AlignConsecutiveDeclarations: False`

## Align escaped newlines as far left as possible

### Example
```cpp
#define MY_MACRO                                                        \
    blablablablablablablabla blablablablablablablablablablablablablabla \
        blablablablablablablablablablablablablablablabla

```
### Clang-Format configuration
* `AlignEscapedNewlinesLeft: True`

## Horizontally align operands of binary and ternary expressions.

### Example
```cpp
int aaa = bbbbbbbbbbbbbbb +
          ccccccccccccccc;

```
### Clang-Format configuration
* `AlignOperands: True`

## Align trailing comments, prefixed with 2 spaces

### Example
```cpp
int a;      // My comment for a
int b = 2;  // comment b

```
### Clang-Format configuration
* `AlignTrailingComments: True`

## Never contract short blocks on a single line

### Example
```cpp
if (a) {
    return;
}

```
### Clang-Format configuration
* `AllowShortBlocksOnASingleLine: False`

## Never contract case labels on a single line

### Example
```cpp
switch (a) {
case 1:
    x = 1;
    break;
case 2:
    return;
case 3: return;  // NOT THIS
}

```
### Clang-Format configuration
* `AllowShortCaseLabelsOnASingleLine: False`

## Only empty methods can be put in a single line

### Example
```cpp
class Foo {
    void f() {
        foo();
    }

    void f() {}
};

```
### Clang-Format configuration
* `AllowShortFunctionsOnASingleLine: Empty`

## Conditions
Break even for simple `if` statement and always put body inside braces.

See clang-tidy check
[readability-braces-around-statements](http://releases.llvm.org/7.0.0/tools/clang/tools/extra/docs/clang-tidy/checks/readability-braces-around-statements.html#readability-braces-around-statements)

### Example
```cpp
if (a) {
    return;
}

```
### Clang-Format configuration
* `AllowShortIfStatementsOnASingleLine: False`

## Loop statements
Break even for simple `for` and `while` statements and always put bodies
inside braces.

See clang-tidy check
[readability-braces-around-statements](http://releases.llvm.org/7.0.0/tools/clang/tools/extra/docs/clang-tidy/checks/readability-braces-around-statements.html#readability-braces-around-statements)

### Example
```cpp
while (true) {
    continue;
}

```
### Clang-Format configuration
* `AllowShortLoopsOnASingleLine: False`

## Break before multiline strings

### Example
```cpp
AAAA =
    "bbbb"
    "cccc";

```
### Clang-Format configuration
* `AlwaysBreakBeforeMultilineStrings: True`

## Break after template declaration

### Example
```cpp
template <typename T>
T foo() {}

template <typename T>
T foo(int aaaaaaaaaaaaaaaaaaaaa, int bbbbbbbbbbbbbbbbbbbbb) {}

```
### Clang-Format configuration
* `AlwaysBreakTemplateDeclarations: True`

## Opening brace is on same line than struct/class definition

### Example
```cpp
class Foo {};

struct Bar {
    int i;
};

```
### Clang-Format configuration
* `BraceWrapping.AfterClass: False`

## Brace is on same line than control statements

### Example
```cpp
if (foo()) {
} else {
}
for (int i = 0; i < 10; ++i) {
}

```
### Clang-Format configuration
* `BraceWrapping.AfterControlStatement: False`

## Brace on same line than *enum* definition

### Example
```cpp
enum X : int { A, B, C };

```
### Clang-Format configuration
* `BraceWrapping.AfterEnum: False`

## Brace on same line than `extern "C"` directive

### Example
```cpp
extern "C" {
int foo();
}

```
### Clang-Format configuration
* `BraceWrapping.AfterExternBlock: False`

## Brace on same line than *function* definitions

### Example
```cpp
void foo() {
    bar();
    bar2();
}

```
### Clang-Format configuration
* `BraceWrapping.AfterFunction: False`

## Brace on same line than *namespace* definition

### Example
```cpp
namespace {
int foo();
int bar();
}  // namespace

```
### Clang-Format configuration
* `BraceWrapping.AfterNamespace: False`

## Brace on same line than *struct* definitions

### Example
```cpp
struct foo {
    int x;
};

```
### Clang-Format configuration
* `BraceWrapping.AfterStruct: False`

## Brace on same line than *union* definitions

### Example
```cpp
union foo {
    int x;
}

```
### Clang-Format configuration
* `BraceWrapping.AfterUnion: False`

## Do not break before *catch* directive

### Example
```cpp
try {
    foo();
} catch () {
}

```
### Clang-Format configuration
* `BraceWrapping.BeforeCatch: False`

## Do not break before *else* block

### Example
```cpp
if (foo()) {
} else {
}

```
### Clang-Format configuration
* `BraceWrapping.BeforeElse: False`

## Do not break before binary operators
When a break is required, the binary operator is the last
token of the line.

### Example
```cpp
LooooooooooongType loooooooooooooooooooooongVariable = someLooooooooooooooooongFunction();

bool value =
    aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa + aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa ==
        aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa &&
    aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa > ccccccccccccccccccccccccccccccccccccccccc;

```
### Clang-Format configuration
* `BreakBeforeBinaryOperators: False`

## break before long ternary operators

### Example
```cpp
veryVeryVeryVeryVeryVeryVeryVeryVeryVeryVeryLongDescription ? firstValue
                                                            : SecondValueVeryVeryVeryVeryLong;

```
### Clang-Format configuration
* `BreakBeforeTernaryOperators: True`

## No space inside C++11 braced lists

### Example
```cpp
vector<int> x{1, 2, 3, 4};
vector<T> x{{}, {}, {}, {}};
f(MyMap[{composite, key}]) new int[3]{1, 2, 3};

```
### Clang-Format configuration
* `Cpp11BracedListStyle: True`

## Recall namespace when closing it

### Example
```cpp
namespace bbp {
foo();
}

```

## Use 4 columns for indentation

### Example
```cpp
void f() {
    someFunction();
    if (true, false) {
        f();
    }
}

```
### Clang-Format configuration
* `IndentWidth: 4`

## Do not indent when entering a *namespace*

### Example
```cpp
namespace out {
int i;
namespace in {
int i;
}
}  // namespace out

```
### Clang-Format configuration
* `NamespaceIndentation: None`

## pointer and reference are part of the type

### Example
```cpp
int* a;
int& b = getB();

```
### Clang-Format configuration
* `PointerAlignment: Left`

## Sort includes in different sections
* System headers
* Dependencies
* Current project

### Example
```cpp
#include <algorithm>
#include <vector>

#include <pybind11/numpy.h>
#include <spdlog/spdlog.h>

#include "mylib/myclass.h"

```
### Clang-Format configuration
* `SortIncludes: True`

## Spacing related specifications
* Use 4 tab size
* Never use tab

### Example
```cpp
// a space is inserted after C style casts
(int) i;

// a space is inserted after the template keyword
template <int>
void foo();

// a space a space before assignment operators
int a = 5;
a += 42;

// do not put spaces before C++11 braced list
Foo foo{bar};
Foo{};
vector<int>{1, 2, 3};
new int[3]{1, 2, 3};

// do not add space before constructor initializer colon
Foo::Foo()
    : a(a) {}

// do not add space before inheritance colon
class Foo: Bar {
}

// put a space before opening parentheses after control statement
// (for/if/while...)
void f() {
    if (true) {
        f();
    }
}

// do not add space before ranged-based for loop colon
for (auto v: values) {
}

// do not have space between `()`
void f() {
    int x[] = {foo(), bar()};
    if (true) {
        f();
    }
}

// put {{ cf['SpacesBeforeTrailingComments']}} spaces before trailing comment

void f() {
    if (true) {  // foo1
        f();     // bar
    }            // foo
}

// do not add space after `<` and before `>`
static_cast<int>(arg);
std::function<void(int)> fct;

// do not add space inside C-style cast
x = (int32) y;

// do not add space insert `(` and before `)`
t f(Deleted&) & = delete;

// do not add space after `[` and before `]`
int a[5];

```
### Clang-Format configuration
* `SpaceAfterCStyleCast: True`
* `SpaceAfterTemplateKeyword: True`
* `SpaceBeforeAssignmentOperators: True`
* `SpaceBeforeCpp11BracedList: False`
* `SpaceBeforeCtorInitializerColon: False`
* `SpaceBeforeInheritanceColon: False`
* `SpaceBeforeParens: ControlStatements`
* `SpaceBeforeRangeBasedForLoopColon: False`
* `SpaceInEmptyParentheses: False`
* `SpacesBeforeTrailingComments: 2`
* `SpacesInAngles: False`
* `SpacesInCStyleCastParentheses: False`
* `SpacesInContainerLiterals: False`
* `SpacesInParentheses: False`
* `SpacesInSquareBrackets: False`
* `TabWidth: 4`
* `UseTab: Never`

## One parameter per line if they don't all fit

### Example
```cpp
HighFive::DataSet dump(HighFive::File& file,
                       const std::string& path,
                       const std::vector<T>& data,
                       HighFive::Mode mode = HighFive::Mode::Create);

void andALongFunctionName(the_long_parameter1, the_long_parameter2);

```
### Clang-Format configuration
* `AllowAllParametersOfDeclarationOnNextLine: False`
* `BinPackParameters: False`


# IDE Configuration

## JetBrains CLion

CLion allows you to disable their formatting engine to get
setting from CLangFormat:
in `Settings > Editor > Code Style > C/C++`, enable
`Enable ClangFormat support (only for C/C++/ObjectiveC)` checkbox.