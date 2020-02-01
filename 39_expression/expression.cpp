#include <sstream>

#include "Common.h"

class BinaryExpression : public Expression {
 private:
  ExpressionPtr lhs;
  ExpressionPtr rhs;
  const char sign;

 public:
  BinaryExpression(ExpressionPtr lhs, ExpressionPtr rhs, char sign)
      : lhs(std::move(lhs)), rhs(std::move(rhs)), sign(sign) {}

  int Evaluate() const override {
    return BinaryEvaluation(lhs->Evaluate(), rhs->Evaluate());
  }

  std::string ToString() const override {
    std::ostringstream os;
    auto AddBraced = [&os](std::string s) mutable { os << '(' << s << ')'; };
    AddBraced(lhs->ToString());
    os << sign;
    AddBraced(rhs->ToString());
    return os.str();
  }

 protected:
  virtual int BinaryEvaluation(int lv, int rv) const = 0;
};

class OperationSum : public BinaryExpression {
  int BinaryEvaluation(int lv, int rv) const override { return lv + rv; }

 public:
  OperationSum(ExpressionPtr lhs, ExpressionPtr rhs)
      : BinaryExpression(std::move(lhs), std::move(rhs), '+') {}
};

class OperationProduct : public BinaryExpression {
  int BinaryEvaluation(int lv, int rv) const override { return lv * rv; }

 public:
  OperationProduct(ExpressionPtr lhs, ExpressionPtr rhs)
      : BinaryExpression(std::move(lhs), std::move(rhs), '*') {}
};

class ValueExpression : public Expression {
 private:
  int value;

 public:
  ValueExpression(int value) : value(value) {}

  int Evaluate() const override { return value; }

  std::string ToString() const override { return std::to_string(value); }
};

ExpressionPtr Value(int value) {
  return std::make_unique<ValueExpression>(value);
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
  return std::make_unique<OperationSum>(std::move(left), std::move(right));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
  return std::make_unique<OperationProduct>(std::move(left), std::move(right));
}