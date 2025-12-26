#include "inline_parser.hpp"
#include "token.hpp"

#include <iterator>
#include <memory>
#include <string>

namespace mt {

std::optional<size_t> InlineParser::find_next(std::span<const Token> tokens,
                                              TokenType type,
                                              size_t start_index) {
  for (size_t seeker_index = start_index; seeker_index < tokens.size();
       ++seeker_index) {
    if (tokens[seeker_index].type == type)
      return seeker_index;
  }

  return std::nullopt;
}

std::string InlineParser::extract_text(std::span<const Token> tokens,
                                       size_t index_start, size_t index_end) {
  std::string text;

  if (index_end > tokens.size())
    index_end = tokens.size() - 1;

  for (size_t index = index_start; index < index_end; ++index) {
    text += tokens[index].literal;
  }

  return text;
}

bool InlineParser::match(std::span<const Token> tokens, size_t index,
                         TokenType type) {
  return index < tokens.size() && tokens[index].type == type;
}

std::vector<std::unique_ptr<Node>>
InlineParser::parse(std::span<const Token> tokens) {
  std::vector<std::unique_ptr<Node>> nodes;
  std::string text_buffer;

  auto construct_text_node = [&]() {
    if (!text_buffer.empty()) {
      nodes.push_back(std::make_unique<Text>(text_buffer));
      text_buffer.clear();
    }
  };

  for (size_t index = 0; index < tokens.size(); ++index) {
    const Token &token = tokens[index];

    // Inline Code: `...`
    if (token.type == TokenType::BACKTICK) {
      // Searching for the closing ` char
      if (auto try_close = find_next(tokens, TokenType::BACKTICK, index + 1)) {
        size_t closing_index = try_close.value();
        construct_text_node();

        std::string contents = extract_text(tokens, index + 1, closing_index);

        nodes.push_back(std::make_unique<InlineCode>(contents));
        index = closing_index;
        continue;
      }
    }

    // Image: ![alt](url)
    if (token.type == TokenType::BANG &&
        match(tokens, index + 1, TokenType::SQR_BRACKET_OPEN)) {

      if (auto try_close =
              find_next(tokens, TokenType::SQR_BRACKET_CLOSE, index + 2)) {
        size_t sqr_bracket_close_index = try_close.value();

        if (match(
                tokens, sqr_bracket_close_index + 1, TokenType::PARENT_OPEN)) {
          if (auto try_close = find_next(tokens,
                                         TokenType::PARENT_CLOSE,
                                         sqr_bracket_close_index + 2)) {
            size_t parent_close_index = try_close.value();

            construct_text_node();

            std::string alt =
                extract_text(tokens, index + 2, sqr_bracket_close_index);
            std::string url = extract_text(
                tokens, sqr_bracket_close_index + 2, parent_close_index);

            nodes.push_back(std::make_unique<Image>(url, alt));
            index = parent_close_index;

            continue;
          }
        }
      }
    }

    // Link: [text](url)
    if (token.type == TokenType::SQR_BRACKET_OPEN) {
      if (auto try_close =
              find_next(tokens, TokenType::SQR_BRACKET_CLOSE, index + 1)) {
        size_t sqr_bracket_close_index = try_close.value();

        if (match(
                tokens, sqr_bracket_close_index + 1, TokenType::PARENT_OPEN)) {
          if (auto try_close = find_next(tokens,
                                         TokenType::PARENT_CLOSE,
                                         sqr_bracket_close_index + 2)) {
            size_t parent_close_index = try_close.value();

            construct_text_node();

            std::string url = extract_text(
                tokens, sqr_bracket_close_index + 2, parent_close_index);

            auto link = std::make_unique<Link>(url);
            if (sqr_bracket_close_index > index + 1) {
              // Recursive parsing of the link text
              link->children = parse(tokens.subspan(
                  index + 1, sqr_bracket_close_index - (index + 1)));
            }

            nodes.push_back(std::move(link));
            index = parent_close_index;
            continue;
          }
        }
      }
    }

    // Emphasis and strong emphasis (* or **)
    if (token.type == TokenType::STAR) {
      bool strong_emphasis = match(tokens, index + 1, TokenType::STAR);
      size_t delimiter_offset = strong_emphasis ? 2 : 1;

      std::optional<size_t> end_index;
      size_t seek_index = index + delimiter_offset;

      while (auto try_close = find_next(tokens, TokenType::STAR, seek_index)) {
        size_t closing_index = try_close.value();
        if (strong_emphasis) {
          // Trying to find a matching pair of closing * chars
          if (match(tokens, closing_index + 1, TokenType::STAR)) {
            end_index = closing_index;
            break;
          }

          seek_index = closing_index + 1;
        } else {
          end_index = closing_index;
          break;
        }
      }
      if (end_index) {
        size_t closing_index = end_index.value();
        construct_text_node();

        auto inner_span =
            tokens.subspan(index + delimiter_offset,
                           closing_index - (index + delimiter_offset));

        // Emphasis node
        std::unique_ptr<Node> emphasis;
        if (strong_emphasis)
          emphasis = std::make_unique<StrongEmphasis>();
        else
          emphasis = std::make_unique<Emphasis>();

        emphasis->children = parse(inner_span);
        nodes.push_back(std::move(emphasis));

        index = closing_index + (strong_emphasis ? 1 : 0);
        continue;
      }
    }

    // If not parsed as anything special
    // it's assumed it's a text node
    text_buffer += token.literal;
  }

  construct_text_node();

  return nodes;
}
} // namespace mt
