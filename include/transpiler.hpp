/*
  Transpiler: manages the entire operation of the program
*/
#pragma once

#include <string>

namespace mt {
class Transpiler {
public:
  static int run(int argc, char *argv[]);

private:
  static bool transpile(const std::string &input_path,
                        const std::string &output_path,
                        bool use_default_styling, bool only_body);
};
} // namespace mt
