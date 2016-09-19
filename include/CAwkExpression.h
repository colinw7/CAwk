#ifndef CAWK_EXPRESSION_H
#define CAWK_EXPRESSION_H

#include <COptVal.h>
#include <CAwkTypes.h>

class CAwkVariableRef;

class CAwkExpressionTerm {
 protected:
  friend class CRefPtr<CAwkExpressionTerm>;

  CAwkExpressionTerm() { }

  virtual ~CAwkExpressionTerm() { }

  CAwkExpressionTerm *dup() const { return NULL; }

 public:
  virtual void print(std::ostream &os) const = 0;

  virtual CAwkExpressionTermPtr execute() = 0;

  virtual bool hasValue() const = 0;

  virtual CAwkValuePtr getValue() const = 0;

  friend std::ostream &operator<<(std::ostream &os, const CAwkExpressionTerm &th) {
    th.print(os); return os;
  }
};

class CAwkExpression : public CAwkExpressionTerm {
 private:
  typedef CAwkExpressionTermList TermList;
  typedef CAwkOperatorList       OpStack;

  TermList        termList_;
  bool            value_;
  OpStack         opStack_;
  CAwkOperatorPtr lastOp_;

 protected:
  friend class CRefPtr<CAwkExpression>;

  CAwkExpression();

 ~CAwkExpression() { }

  CAwkExpression *dup() const { return new CAwkExpression(*this); }

 public:
  static CAwkExpressionPtr create() {
    return CAwkExpressionPtr(new CAwkExpression);
  }

  bool hasValue() const { return true; }

  CAwkValuePtr getValue() const;

  void pushTerm(CAwkExpressionTermPtr term);

  bool isValue() const { return value_; }

  uint numTerms() const { return termList_.size(); }

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkExpression &th) {
    th.print(os); return os;
  }
};

class CAwkGetLineExpr : public CAwkExpressionTerm {
 private:
  CAwkVariableRefPtr var_;
  CAwkIFilePtr       file_;

 public:
  static CAwkExpressionTermPtr
  create(CAwkVariableRefPtr var, CAwkIFilePtr file) {
    return CAwkExpressionTermPtr(new CAwkGetLineExpr(var, file));
  }

 private:
  CAwkGetLineExpr(CAwkVariableRefPtr var,
                  CAwkIFilePtr file) :
   var_(var), file_(file) {
  }

 public:
  bool hasValue() const { return true; }

  CAwkValuePtr getValue() const;

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const;
};

#endif
