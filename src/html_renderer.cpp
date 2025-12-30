#include "html_renderer.hpp"
#include "node.hpp"
#include <utility>

namespace mt {

static const std::string default_optional_style =
    "body { "
    "  font-family: 'Times New Roman', serif; "
    "  background-color: #ffffff; "
    "  color: #000000; "
    "  margin: 1em; "
    "}\n"
    "blockquote { "
    "  margin: 0.25em 0 1em 0.25em; "
    "  padding-left: 0.5em; "
    "  border-left: 3px solid #ccc; "
    "  color: #444; "
    "}\n"
    "code { "
    "  background-color: #f5f5f5; "
    "  padding: 2px 4px; "
    "  border-radius: 3px; "
    "}\n"
    "pre { "
    "  background-color: #f5f5f5; "
    "  padding: 1em; "
    "  border-radius: 3px; "
    "  width: -moz-fit-content; "
    "  width: fit-content; "
    "}\n"
    "pre code { "
    "  background-color: transparent; "
    "  padding: 0; "
    "}\n";

HtmlRenderer::HtmlRenderer(std::string title, bool use_default_style,
                           bool only_body)
    : m_title(std::move(title)),
      m_use_default_style(use_default_style),
      m_only_body(only_body) {
}

std::string HtmlRenderer::get_output() const {
  std::stringstream html_output;

  if (!m_only_body) {

    html_output << "<!DOCTYPE html>\n<html>\n<head>\n<meta "
                   "charset=\"utf-8\">\n<title>";
    html_output << m_title;
    html_output << "</title>\n";
  }

  if (m_use_default_style) {
    html_output << "<style>\n";
    html_output << default_optional_style;
    html_output << "</style>\n";
  }

  if (!m_only_body)
    html_output << "</head>\n";

  html_output << "<body>\n";
  html_output << m_html_body.str();
  html_output << "\n</body>\n";

  if (!m_only_body)
    html_output << "</html>";

  return html_output.str();
}

void HtmlRenderer::clear() {
  m_html_body.clear();
}

// Prevents injecting HTML code/tags from
// the Markdown file
std::string HtmlRenderer::escape_html(const std::string_view data) {
  std::string buffer;
  buffer.reserve(data.size());
  for (char c : data) {
    switch (c) {
    case '&':
      buffer.append("&amp;");
      break;
    case '\"':
      buffer.append("&quot;");
      break;
    case '\'':
      buffer.append("&apos;");
      break;
    case '<':
      buffer.append("&lt;");
      break;
    case '>':
      buffer.append("&gt;");
      break;
    default:
      buffer.push_back(c);
      break;
    }
  }
  return buffer;
}

// actual imp
void HtmlRenderer::visit(const Document &node) {
  for (const auto &child : node.children)
    child->accept(*this);
}

void HtmlRenderer::visit(const Paragraph &node) {
  m_html_body << "<p>";
  for (const auto &child : node.children)
    child->accept(*this);
  m_html_body << "</p>\n";
}

void HtmlRenderer::visit(const Heading &node) {
  m_html_body << "<h" << (int)node.heading_level << ">";
  for (const auto &child : node.children)
    child->accept(*this);
  m_html_body << "</h" << (int)node.heading_level << ">\n";
}

void HtmlRenderer::visit(const CodeSpan &node) {
  m_html_body << "<pre><code";
  if (!node.language.empty()) {
    m_html_body << " class=\"language-" << escape_html(node.language) << "\"";
  }
  m_html_body << ">";
  for (const auto &token : node.tokens)
    m_html_body << escape_html(token.literal);
  m_html_body << "</code></pre>\n";
}

void HtmlRenderer::visit(const Text &node) {
  m_html_body << escape_html(node.text);
}

void HtmlRenderer::visit(const Emphasis &node) {
  m_html_body << "<em>";
  for (const auto &child : node.children)
    child->accept(*this);
  m_html_body << "</em>";
}

void HtmlRenderer::visit(const StrongEmphasis &node) {
  m_html_body << "<strong>";
  for (const auto &child : node.children)
    child->accept(*this);
  m_html_body << "</strong>";
}

void HtmlRenderer::visit(const Link &node) {
  m_html_body << "<a href=\"" << escape_html(node.url) << "\">";
  for (const auto &child : node.children)
    child->accept(*this);
  m_html_body << "</a>";
}

void HtmlRenderer::visit(const Image &node) {
  m_html_body << "<img src=\"" << escape_html(node.url) << "\" alt=\""
              << escape_html(node.alt_text) << "\" />";
}

void HtmlRenderer::visit(const InlineCode &node) {
  m_html_body << "<code>" << escape_html(node.code) << "</code>";
}

void HtmlRenderer::visit(const List &node) {
  m_html_body << "<ul>\n";
  for (const auto &child : node.children)
    child->accept(*this);
  m_html_body << "</ul>\n";
}

void HtmlRenderer::visit(const ListItem &node) {
  m_html_body << "<li>";
  for (const auto &child : node.children)
    child->accept(*this);
  m_html_body << "</li>\n";
}

void HtmlRenderer::visit(const BlockQuote &node) {
  m_html_body << "<blockquote>\n";
  for (const auto &child : node.children)
    child->accept(*this);
  m_html_body << "</blockquote>\n";
}

} // namespace mt