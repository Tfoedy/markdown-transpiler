#pragma once

namespace mt {

struct Document;
struct Paragraph;
struct Heading;
struct CodeSpan;
struct Text;
struct Emphasis;
struct StrongEmphasis;
struct Link;
struct Image;
struct InlineCode;
struct List;
struct ListItem;
struct BlockQuote;

class Visitor {
public:
  virtual ~Visitor() = default;

  virtual void visit(const Document &node) = 0;

  virtual void visit(const Paragraph &node) = 0;
  virtual void visit(const Heading &node) = 0;
  virtual void visit(const CodeSpan &node) = 0;

  virtual void visit(const BlockQuote &node) = 0;

  virtual void visit(const Text &node) = 0;

  virtual void visit(const Emphasis &node) = 0;
  virtual void visit(const StrongEmphasis &node) = 0;

  virtual void visit(const Link &node) = 0;
  virtual void visit(const Image &node) = 0;

  virtual void visit(const InlineCode &node) = 0;

  virtual void visit(const List &node) = 0;
  virtual void visit(const ListItem &node) = 0;
};

} // namespace mt
