#include "transpiler.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "block_parser.hpp"
#include "html_renderer.hpp"
#include "lexer.hpp"

namespace mt {

int Transpiler::run(int argc, char *argv[]) {
  bool use_default_styling = true;
  std::string input_filename;
  std::string output_filename;

  // Parsing the program arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--no-styling") {
      use_default_styling = false;
    } else if (input_filename.empty()) {
      input_filename = arg;
    } else if (output_filename.empty()) {
      output_filename = arg;
    }
  }

  if (input_filename.empty()) {
    std::cout << "Usage: " << argv[0]
              << " <input_file> [output_file] [--no-styling]\n";
    return 1;
  }

  // Getting the filename
  if (output_filename.empty()) {
    std::filesystem::path input_path(input_filename);
    output_filename = input_path.stem().string() + ".html";
  } else {
    std::filesystem::path out_path(output_filename);
    if (!out_path.has_extension()) {
      out_path += ".html";
      output_filename = out_path.string();
    }
  }

  return transpile(input_filename, output_filename, use_default_styling) ? 0
                                                                         : 1;
}

bool Transpiler::transpile(const std::string &input_path,
                           const std::string &output_path,
                           bool use_custom_style) {
  std::ifstream file(input_path);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open input file: " << input_path << "\n";
    return false;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();

  // 1. Lexing
  Lexer lexer(source);
  auto tokens = lexer.tokenize();

  if (tokens.empty() && !source.empty()) {
    std::cerr << "Error: Lexing failed.\n";
    return false;
  }

  // 2. Parsing
  BlockParser parser(tokens);
  auto document = parser.parse();
  if (!document) {
    std::cerr << "Error: Parsing failed.\n";
    return false;
  }

  std::filesystem::path p(input_path);
  std::string doc_title = p.stem().string();

  // 3. Rendering
  HtmlRenderer renderer(doc_title, use_custom_style);
  document->accept(renderer);
  std::string html_content = renderer.get_output();

  // 4. Output
  std::ofstream out_file(output_path);
  if (!out_file.is_open()) {
    std::cerr << "Error: Could not open output file: " << output_path << "\n";
    return false;
  }

  out_file << html_content;
  std::cout << "Successfully transpiled '" << input_path << "' to '"
            << output_path << "'.\n";

  if (!use_custom_style) {
    std::cout << "(Default styling disabled)\n";
  }

  return true;
}

} // namespace mt
