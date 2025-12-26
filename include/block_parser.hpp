/*
  Block Parser: divides and groups tokens into Nodes, that
  represent Markdown blocks and assigns those Nodes their inline tokens for
  further inline parsing
*/
#pragma once

#include <memory>
#include <vector>

#include "node.hpp"
#include "token.hpp"

namespace mt {

class BlockParser {
public:
  explicit BlockParser(const std::vector<Token> &tokens);

  std::unique_ptr<Document> parse();

private:
  // The main dispatching function
  std::unique_ptr<Node> parse_block_dispatch();
  // Individual block parsers
  std::unique_ptr<Paragraph> parse_paragraph();
  std::unique_ptr<Heading> parse_header();
  std::unique_ptr<CodeSpan> parse_code_span();
  std::unique_ptr<BlockQuote> parse_quote();
  // List related
  std::unique_ptr<List> parse_list(size_t indent = 0);
  std::unique_ptr<Node> parse_list_item();
  size_t count_list_indentation(size_t &index_offset) const;

  /* --- */
  bool at_end(size_t offset = 0) const;
  bool is_line_start() const;

  bool check_current_type(TokenType type) const;

  const Token &current_token() const;
  const Token &peek(size_t index_offset) const;

  void advance(size_t index = 1);

private:
  const std::vector<Token> &m_tokens;
  size_t m_index;
};

} // namespace mt
