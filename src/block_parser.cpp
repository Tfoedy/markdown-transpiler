#include "block_parser.hpp"

#include "inline_parser.hpp"
#include "token.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <numeric>

namespace mt {

BlockParser::BlockParser(const std::vector<Token> &tokens)
    : m_tokens(tokens),
      m_index(0) {
}

std::unique_ptr<Document> BlockParser::parse() {
  auto document = std::make_unique<Document>();

  if (m_tokens.empty())
    return document;

  if (check_current_type(TokenType::START_OF_FILE))
    advance();

  while (!at_end()) {
    // Discarding potential initial empty lines
    if (check_current_type(TokenType::NEW_LINE)) {
      advance();
      continue;
    }

    if (auto block = parse_block_dispatch())
      document->children.push_back(std::move(block));
  }

  return document;
}

std::unique_ptr<Node> BlockParser::parse_block_dispatch() {
  // Blocks other than Paragraphs must start on a new line (unless in a list)
  if (is_line_start()) {
    TokenType current_type = current_token().type;
    size_t initial_index = m_index;

    // Header
    if (current_type == TokenType::HASH) {
      if (auto header = parse_header())
        return header;
      m_index = initial_index;
    }

    // Code span
    if (current_type == TokenType::BACKTICK) {
      if (auto code_span = parse_code_span())
        return code_span;
      m_index = initial_index;
    }

    // Quote
    if (current_type == TokenType::GREATER_THAN) {
      if (auto quote = parse_quote())
        return quote;
      m_index = initial_index;
    }

    // List
    if ((current_type == TokenType::STAR ||
         current_type == TokenType::HYPHEN) &&
        peek(1).type == TokenType::SPACE) {
      if (auto list = parse_list())
        return list;
      m_index = initial_index;
    }
  }

  // If not parsed as anything special up to this point,
  // it's assumed it's a paragraph
  return parse_paragraph();
}

// Individual parser implementations
std::unique_ptr<Heading> BlockParser::parse_header() {
  uint8_t hash_char_count = 0;
  while (check_current_type(TokenType::HASH)) {
    hash_char_count++;
    advance();
  }

  if (hash_char_count == 0 || hash_char_count > 6 ||
      !check_current_type(TokenType::SPACE))
    return nullptr;
  advance(); // space

  auto header = std::make_unique<Heading>(hash_char_count);

  while (!at_end() && !check_current_type(TokenType::NEW_LINE)) {
    header->tokens.push_back(current_token());
    advance();
  }

  header->children = InlineParser::parse(header->tokens);

  return header;
}

std::unique_ptr<CodeSpan> BlockParser::parse_code_span() {
  int backtick_char_count = 0;
  while (check_current_type(TokenType::BACKTICK)) {
    backtick_char_count++;
    advance();
  }

  if (backtick_char_count != 3)
    return nullptr;

  std::string language{};
  if (check_current_type(TokenType::TEXT)) {
    language = current_token().literal;
    advance();
  }

  while (check_current_type(TokenType::NEW_LINE))
    advance();

  auto code_span = std::make_unique<CodeSpan>(language);
  while (!at_end()) {
    if (check_current_type(TokenType::BACKTICK) && is_line_start()) {
      if (peek(1).type == TokenType::BACKTICK &&
          peek(2).type == TokenType::BACKTICK) {
        advance(3);
        break;
      }
    }
    code_span->tokens.push_back(current_token());
    advance();
  }

  // Getting rid of a potential empty line at the back
  if (!code_span->tokens.empty() &&
      code_span->tokens.back().type == TokenType::NEW_LINE)
    code_span->tokens.pop_back();

  return code_span;
}

std::unique_ptr<BlockQuote> BlockParser::parse_quote() {
  std::vector<Token> quote_tokens;

  while (!at_end()) {
    size_t quote_index_offset = 0;
    while (peek(quote_index_offset).type == TokenType::SPACE ||
           peek(quote_index_offset).type == TokenType::TAB)
      quote_index_offset++;

    if (peek(quote_index_offset).type == TokenType::GREATER_THAN) {
      advance(quote_index_offset); // SPACE char
      advance();                   // '>' char

      if (check_current_type(TokenType::SPACE))
        advance();

      while (!at_end() && !check_current_type(TokenType::NEW_LINE)) {
        quote_tokens.push_back(current_token());
        advance();
      }
    } else {
      break;
    }
  }

  BlockParser quote_tokens_parser(quote_tokens);
  auto inner_quote_doc = quote_tokens_parser.parse();

  auto quote = std::make_unique<BlockQuote>();
  quote->children = std::move(inner_quote_doc->children);

  return quote;
}

std::unique_ptr<List> BlockParser::parse_list(size_t current_indentation) {
  auto list = std::make_unique<List>();

  while (!at_end()) {
    // Checking the indentation and the list marker (- or *)
    size_t index_offset = 0;
    size_t indentation = count_list_indentation(index_offset);

    // Is it a part of this list
    if (indentation < current_indentation)
      break;

    // Checking if the list grammar is correct
    TokenType list_element_marker = peek(index_offset).type;
    if ((list_element_marker != TokenType::HYPHEN &&
         list_element_marker != TokenType::STAR) ||
        peek(index_offset + 1).type != TokenType::SPACE)
      break;

    // Skipping the indentation, marker and space chars
    advance(index_offset + 2);
    auto list_item = std::make_unique<ListItem>();

    // Item parsing
    bool is_first_line = true;
    while (!at_end()) {
      if (!is_first_line) {
        if (check_current_type(TokenType::NEW_LINE)) {
          advance();
          continue;
        }

        size_t next_item_index_offset = 0;
        size_t next_indentation =
            count_list_indentation(next_item_index_offset);

        // The end of the item of the list
        if (next_indentation <= current_indentation)
          break;

        TokenType next_list_item_marker = peek(next_item_index_offset).type;

        if ((next_list_item_marker == TokenType::HYPHEN ||
             next_list_item_marker == TokenType::STAR) &&
            peek(next_item_index_offset + 1).type == TokenType::SPACE) {
          list_item->children.push_back(parse_list(next_indentation));
          continue;
        }

        advance(next_item_index_offset);
      }

      list_item->children.push_back(parse_list_item());

      if (check_current_type(TokenType::NEW_LINE))
        advance();
      is_first_line = false;
    }

    list->children.push_back(std::move(list_item));
  }

  return list;
}

std::unique_ptr<Node> BlockParser::parse_list_item() {
  size_t initial_index = m_index;

  // Header
  if (check_current_type(TokenType::HASH)) {
    if (auto header = parse_header())
      return header;
    m_index = initial_index;
  }

  // Quote
  if (check_current_type(TokenType::GREATER_THAN)) {
    if (auto quote = parse_quote())
      return quote;
    m_index = initial_index;
  }

  // Code Span
  if (check_current_type(TokenType::BACKTICK)) {
    if (auto code_span = parse_code_span())
      return code_span;
    m_index = initial_index;
  }

  // If not a special list item,
  // it's assumed it is a paragraph
  auto paragraph = std::make_unique<Paragraph>();
  while (!at_end() && !check_current_type(TokenType::NEW_LINE)) {
    paragraph->tokens.push_back(current_token());
    advance();
  }

  paragraph->children = InlineParser::parse(paragraph->tokens);

  return paragraph;
}

size_t BlockParser::count_list_indentation(size_t &index_offset) const {
  size_t indentation = 0;
  index_offset = 0;

  while (peek(index_offset).type == TokenType::SPACE ||
         peek(index_offset).type == TokenType::TAB) {
    indentation += (peek(index_offset).type == TokenType::TAB ? 4 : 1);
    index_offset++;
  }

  return indentation;
}

std::unique_ptr<Paragraph> BlockParser::parse_paragraph() {
  auto paragraph = std::make_unique<Paragraph>();

  while (!at_end()) {
    if (check_current_type(TokenType::NEW_LINE)) {
      if (peek(-1).type == TokenType::NEW_LINE) {
        advance();
        break;
      }

      TokenType next_token_type = peek(1).type;

      if ((next_token_type == TokenType::HYPHEN ||
           next_token_type == TokenType::STAR) &&
          peek(2).type == TokenType::SPACE) {
        advance();
        break;
      }

      if (next_token_type == TokenType::GREATER_THAN) {
        advance();
        break;
      }

      if (next_token_type == TokenType::HASH) {
        advance();
        break;
      }

      if (next_token_type == TokenType::BACKTICK) {
        advance();
        break;
      }

      paragraph->tokens.push_back(current_token());
      advance();
      continue;
    }
    paragraph->tokens.push_back(current_token());
    advance();
  }

  paragraph->children = InlineParser::parse(paragraph->tokens);

  return paragraph;
}

bool BlockParser::at_end(size_t index_offset) const {
  return (m_index + index_offset) >= m_tokens.size() ||
         m_tokens[m_index + index_offset].type == TokenType::END_OF_FILE;
}

const Token &BlockParser::current_token() const {
  return m_tokens[m_index];
}

const Token &BlockParser::peek(size_t index_offset) const {
  size_t peek_index = m_index + index_offset;
  if (m_tokens.empty()) {
    if (!m_tokens.empty())
      return m_tokens.front();
    static Token token = Token();
    return token;
  }
  if (peek_index >= m_tokens.size()) {
    return m_tokens.back();
  }

  return m_tokens[peek_index];
}

void BlockParser::advance(size_t n) {
  m_index = std::min(m_index + n, m_tokens.size());
}

bool BlockParser::check_current_type(TokenType type) const {
  return !at_end() && current_token().type == type;
}

bool BlockParser::is_line_start() const {
  if (m_index == 0)
    return true;
  TokenType previous_type = peek(-1).type;
  return previous_type == TokenType::NEW_LINE ||
         previous_type == TokenType::START_OF_FILE;
}

} // namespace mt
