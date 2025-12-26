/*
  HTML Renderer: constructs the HTML code based on the AST
*/

#pragma once

#include "visitor.hpp"
#include <sstream>
#include <string>

namespace mt {

class HtmlRenderer : public Visitor {
public:
  explicit HtmlRenderer(std::string title, bool use_default_style = true);

  std::string get_output() const;
  void clear();

  void visit(const Document &node) override;
  void visit(const Paragraph &node) override;
  void visit(const Heading &node) override;
  void visit(const CodeSpan &node) override;
  void visit(const Text &node) override;
  void visit(const Emphasis &node) override;
  void visit(const StrongEmphasis &node) override;
  void visit(const Link &node) override;
  void visit(const Image &node) override;
  void visit(const InlineCode &node) override;
  void visit(const List &node) override;
  void visit(const ListItem &node) override;
  void visit(const BlockQuote &node) override;

private:
  std::string escape_html(const std::string_view data);

private:
  std::stringstream m_html_body;
  std::string m_title;
  bool m_use_default_style;
};

} // namespace mt
