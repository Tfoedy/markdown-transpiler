/*
  Token: represents a special Markdown character or text
*/
#pragma once

#include <string_view>

namespace mt {
enum class TokenType {
  START_OF_FILE = 0,
  END_OF_FILE,
  NEW_LINE,
  TAB,
  SPACE,
  TEXT,
  HASH,
  BANG,
  HYPHEN,
  BACKTICK,
  STAR,
  GREATER_THAN,
  BACKSLASH,
  PARENT_OPEN,
  PARENT_CLOSE,
  SQR_BRACKET_OPEN,
  SQR_BRACKET_CLOSE
};

struct Token {
  TokenType type;
  std::string_view literal;
  size_t line_at;
};

} // namespace mt
