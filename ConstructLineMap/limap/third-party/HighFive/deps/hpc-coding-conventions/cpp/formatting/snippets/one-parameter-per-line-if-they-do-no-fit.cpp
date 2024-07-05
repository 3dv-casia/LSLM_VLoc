// One parameter per line if they don't all fit
// clang-format: AllowAllParametersOfDeclarationOnNextLine: false
// clang-format: BinPackParameters: False

HighFive::DataSet dump(HighFive::File& file,
                       const std::string& path,
                       const std::vector<T>& data,
                       HighFive::Mode mode = HighFive::Mode::Create);

void andALongFunctionName(the_long_parameter1, the_long_parameter2);
