/*
这个程序用来表示算术表达式的树，比如表达式(-5)*(3 + 4) 对应的树为：
      *
    /   \
   -     +
  /     /  \
 5     3    4

一个表达式树包括代表常数、一元运算符和二元运算符的节点。
要求能通过调用合适的函数来创建这样的树，然后打印该树的完整括号化形式。

可以用面向对象的方式来解决问题
*/

#include <iostream>
#include <string>

// forward declaration
class Expression;

// 基类节点
class ExpressionNode {
 public:
  friend std::ostream& operator<<(std::ostream& o,
                                  const ExpressionNode& expr_node) {
    expr_node.print(o);
    return o;
  }
  friend class Expression;
  int use_;

 protected:
  ExpressionNode() : use_(1) {}
  virtual void print(std::ostream&) const = 0;
  virtual ~ExpressionNode() {}
  virtual int eval() const = 0;
};

// Expression Class is actually a Handle class, it contains the Nodes
class Expression {
 private:
  friend class ExpressionNode;
  friend std::ostream& operator<<(std::ostream& o, const Expression& expr) {
    expr.p_->print(o);
    return o;
  }

  ExpressionNode* p_;

 public:
  Expression(int n);
  Expression(const std::string& op, Expression expr);
  Expression(const std::string& op, Expression left, Expression right);
  Expression(const Expression& e);

  ~Expression();

  Expression& operator=(const Expression& e);
  int eval() const;
};

// derived Node class
class IntNode : public ExpressionNode {
 private:
  friend class Expression;

  int n_;
  IntNode(int k) : n_(k) {}
  void print(std::ostream& o) const { o << n_; }
  int eval() const { return n_; }
};

class UnaryNode : public ExpressionNode {
 private:
  friend class Expression;
  std::string operator_;
  Expression operator_node_;

  UnaryNode(const std::string& op, Expression node)
      : operator_(op), operator_node_(node) {}
  void print(std::ostream& o) const {
    o << "(" << operator_ << operator_node_ << ")";
  }
  int eval() const {
    if (operator_ == "-") {
      return -operator_node_.eval();
    }
    throw "error, bad op " + operator_ + " int UnaryNode";
  }
};

class BinaryNode : public ExpressionNode {
 private:
  friend class Expression;
  std::string operator_;
  Expression left_node_;
  Expression right_node_;

  BinaryNode(const std::string& op, Expression left_node, Expression right_node)
      : operator_(op), left_node_(left_node), right_node_(right_node) {}
  void print(std::ostream& o) const {
    o << "(" << left_node_ << operator_ << right_node_ << ")";
  }
  int eval() const {
    int op1 = left_node_.eval();
    int op2 = right_node_.eval();
    if (operator_ == "+") {
      return op1 + op2;
    }
    if (operator_ == "-") {
      return op1 - op2;
    }
    if (operator_ == "*") {
      return op1 * op2;
    }
    if (operator_ == "/" && op2 != 0) {
      return op1 / op2;
    }
    throw "error, bad op " + operator_ + " in BinaryNode";
  }
};

Expression::Expression(int n) : p_(new IntNode(n)) {}
Expression::Expression(const std::string& op, Expression expr) {
  p_ = new UnaryNode(op, expr);
}
Expression::Expression(const std::string& op, Expression left,
                       Expression right) {
  p_ = new BinaryNode(op, left, right);
}
Expression::Expression(const Expression& e) {
  p_ = e.p_;
  ++p_->use_;
}

Expression::~Expression() {
  if (--p_->use_ == 0) {
    delete p_;
  }
}

Expression& Expression::operator=(const Expression& e) {
  e.p_->use_++;
  if (--p_->use_ == 0) {
    delete p_;
  }
  p_ = e.p_;
  return *this;
}

int Expression::eval() const { return p_->eval(); }

int main() {
  Expression t = Expression("*", Expression("-", 5), Expression("+", 3, 4));
  std::cout << t << " = " << t.eval() << std::endl;

  t = Expression("*", t, t);
  std::cout << t << " = " << t.eval() << std::endl;

  return 0;
}