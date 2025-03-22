#ifndef CAWK_EXPRESSION_H
#define CAWK_EXPRESSION_H

#include <CAwkTypes.h>

class CAwkVariableRef;

class CAwkExpressionTerm {
 protected:
  CAwkExpressionTerm() { }

  CAwkExpressionTerm *dup() const { return NULL; }

 public:
  virtual ~CAwkExpressionTerm() { }

  virtual void print(std::ostream &os) const = 0;

  virtual CAwkExpressionTermPtr execute() = 0;

  virtual bool hasValue() const = 0;

  virtual CAwkValuePtr getValue() const = 0;

  friend std::ostream &operator<<(std::ostream &os, const CAwkExpressionTerm &th) {
    th.print(os); return os;
  }
};

//---

class CAwkExpression : public CAwkExpressionTerm {
 protected:
  CAwkExpression();

  CAwkExpression *dup() const { return new CAwkExpression(*this); }

 public:
  static CAwkExpressionPtr create() {
    return CAwkExpressionPtr(new CAwkExpression);
  }

 ~CAwkExpression();

  bool hasValue() const override { return true; }

  CAwkValuePtr getValue() const override;

  void pushTerm(CAwkExpressionTermPtr term);

  bool isValue() const { return value_; }

  uint numTerms() const { return uint(termList_.size()); }

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override;

  friend std::ostream &operator<<(std::ostream &os, const CAwkExpression &th) {
    th.print(os); return os;
  }

 private:
  using TermList = CAwkExpressionTermList;
  using OpStack  = CAwkOperatorList;

  TermList        termList_;
  bool            value_ { false };
  OpStack         opStack_;
  CAwkOperatorPtr lastOp_;
};

//---

class CAwkGetLineExpr : public CAwkExpressionTerm {
 public:
  static CAwkExpressionTermPtr create(CAwkVariableRefPtr var, CAwkIFilePtr file, bool hasValue) {
    return CAwkExpressionTermPtr(new CAwkGetLineExpr(var, file, hasValue));
  }

 private:
  CAwkGetLineExpr(CAwkVariableRefPtr var, CAwkIFilePtr file, bool hasValue) :
   var_(var), file_(file), hasValue_(hasValue) {
  }

 public:
  bool hasValue() const override { return hasValue_; }

  CAwkValuePtr getValue() const override;

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override;

 private:
  CAwkVariableRefPtr var_;
  CAwkIFilePtr       file_;
  bool               hasValue_ { false };
};

#endif
