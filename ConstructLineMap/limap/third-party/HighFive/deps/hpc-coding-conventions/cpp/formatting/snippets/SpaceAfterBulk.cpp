// Spacing related specifications
// clang-format: SpaceAfterCStyleCast
// clang-format: SpaceAfterTemplateKeyword
// clang-format: SpaceBeforeAssignmentOperators
// clang-format: SpaceBeforeCpp11BracedList
// clang-format: SpaceBeforeCtorInitializerColon
// clang-format: SpaceBeforeInheritanceColon
// clang-format: SpaceBeforeParens
// clang-format: SpaceBeforeRangeBasedForLoopColon
// clang-format: SpaceInEmptyParentheses
// clang-format: SpacesBeforeTrailingComments
// clang-format: SpacesInAngles
// clang-format: SpacesInCStyleCastParentheses
// clang-format: SpacesInContainerLiterals
// clang-format: SpacesInParentheses
// clang-format: SpacesInSquareBrackets
// clang-format: TabWidth
// clang-format: UseTab
// * Use {{ cf["TabWidth"]}} tab size
// * Never use tab

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
