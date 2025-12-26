/*
  Inline Parser: divides and groups tokens from a single line into Nodes, that
  represent Markdown inline elements
*/
#pragma once

#include <memory>
#include <optional>
#include <span>
#include <vector>

#include "node.hpp"
#include "token.hpp"

namespace mt {
class InlineParser {
public:
  static std::optional<size_t> find_next(std::span<const Token> tokens,
                                         TokenType type, size_t start_index);
  static std::string extract_text(std::span<const Token> tokens,
                                  size_t index_start, size_t index_end);
  static bool match(std::span<const Token> tokens, size_t index,
                    TokenType type);

  static std::vector<std::unique_ptr<Node>>
  parse(std::span<const Token> tokens);
};
} // namespace mt
