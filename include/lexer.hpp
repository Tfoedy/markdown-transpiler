/*
  Lexer: divides and groups input Markdown text into tokens, which represent
  additional meaning needed for easier parsing
*/

#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "token.hpp"

namespace mt {

class Lexer {
public:
  explicit Lexer(std::string_view source);

  std::vector<Token> tokenize();

private:
  void add_token(TokenType type, std::string_view literal, size_t line);
  bool at_eof() const;
  std::optional<size_t> find_next_special() const;

private:
  std::string_view m_source;
  std::vector<Token> m_tokens;

  size_t m_index;
  size_t m_line;
};

} // namespace mt
