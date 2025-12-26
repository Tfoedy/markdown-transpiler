/*
  Node: represents a whole Markdown block or inline element.
*/
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "token.hpp"
#include "visitor.hpp"

namespace mt {

struct Node {
  virtual ~Node() = default;

  virtual void accept(Visitor &visitor) const = 0;

  std::vector<std::unique_ptr<Node>> children;
};

struct Document : Node {
  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct Paragraph : Node {
  std::vector<Token> tokens;

  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct Heading : Node {
  explicit Heading(uint8_t level)
      : heading_level(level) {
  }

  uint8_t heading_level;
  std::vector<Token> tokens;

  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct CodeSpan : Node {
  explicit CodeSpan(std::string lang)
      : language(std::move(lang)) {
  }

  std::string language;
  std::vector<Token> tokens;

  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct Text : Node {
  explicit Text(std::string text)
      : text(std::move(text)) {
  }

  std::string text;

  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct Emphasis : Node {
  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct StrongEmphasis : Node {
  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct Link : Node {
  explicit Link(std::string url)
      : url(std::move(url)) {
  }

  std::string url;

  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct Image : Node {
  Image(std::string url, std::string alt)
      : url(std::move(url)),
        alt_text(std::move(alt)) {
  }

  std::string url;
  std::string alt_text;

  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct InlineCode : Node {
  explicit InlineCode(std::string code_text)
      : code(std::move(code_text)) {
  }
  std::string code;

  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct List : Node {
  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct ListItem : Node {
  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

struct BlockQuote : Node {
  void accept(Visitor &visitor) const override {
    visitor.visit(*this);
  }
};

} // namespace mt
