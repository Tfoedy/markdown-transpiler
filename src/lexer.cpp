#include "lexer.hpp"

namespace mt {

Lexer::Lexer(std::string_view source)
    : m_source(source),
      m_tokens(),
      m_index(0),
      m_line(1) {
}

std::vector<Token> Lexer::tokenize() {
  m_tokens.clear();
  m_index = 0;
  m_line = 1;

  add_token(TokenType::START_OF_FILE, "", m_line);

  while (!at_eof()) {
    char character = m_source[m_index];
    size_t seeker_index = m_index;

    if (character == '\r') {
      m_index++;
      continue;
    }

    switch (character) {
    case ' ':
      add_token(TokenType::SPACE, " ", m_line);
      break;
    case '\n':
      add_token(TokenType::NEW_LINE, "\n", m_line++);
      break;
    case '\t':
      add_token(TokenType::TAB, "\t", m_line);
      break;
    case '#':
      add_token(TokenType::HASH, "#", m_line);
      break;
    case '!':
      add_token(TokenType::BANG, "!", m_line);
      break;
    case '-':
      add_token(TokenType::HYPHEN, "-", m_line);
      break;
    case '`':
      add_token(TokenType::BACKTICK, "`", m_line);
      break;
    case '*':
      add_token(TokenType::STAR, "*", m_line);
      break;
    case '>':
      add_token(TokenType::GREATER_THAN, ">", m_line);
      break;
    case '\\':
      add_token(TokenType::BACKSLASH, "\\", m_line);
      break;
    case '(':
      add_token(TokenType::PARENT_OPEN, "(", m_line);
      break;
    case ')':
      add_token(TokenType::PARENT_CLOSE, ")", m_line);
      break;
    case '[':
      add_token(TokenType::SQR_BRACKET_OPEN, "[", m_line);
      break;
    case ']':
      add_token(TokenType::SQR_BRACKET_CLOSE, "]", m_line);
      break;

    default: {
      auto find_special_char = find_next_special();
      size_t special_char_index = find_special_char.value_or(m_source.size());

      if (special_char_index <= seeker_index)
        special_char_index = seeker_index + 1;

      std::string_view text =
          m_source.substr(seeker_index, special_char_index - seeker_index);
      add_token(TokenType::TEXT, text, m_line);

      m_index = special_char_index - 1;
      break;
    }
    }

    m_index++;
  }

  add_token(TokenType::END_OF_FILE, "", m_line);
  return m_tokens;
}

void Lexer::add_token(TokenType type, std::string_view literal, size_t line) {
  m_tokens.push_back(Token{type, literal, line});
}

bool Lexer::at_eof() const {
  return m_index >= m_source.size();
}

std::optional<size_t> Lexer::find_next_special() const {
  static constexpr std::string_view special_chars = "\n\r#!-`*>()[\\\\]";

  size_t find = m_source.find_first_of(special_chars, m_index);

  if (find == std::string_view::npos)
    return std::nullopt;

  return find;
}

} // namespace mt
