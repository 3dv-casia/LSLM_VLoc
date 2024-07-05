// Never contract case labels on a single line

switch (a) {
case 1:
    x = 1;
    break;
case 2:
    return;
// clang-format off
case 3: return;  // NOT THIS
    // clang-format on
}
