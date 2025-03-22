#ifndef CAWK_EXECUTE_STACK_H
#define CAWK_EXECUTE_STACK_H

#include <CAwkTypes.h>

class CAwkExecuteSubStack;
class CAwkVariableRef;

class CAwkExecuteStack {
 public:
  CAwkExecuteStack();
 ~CAwkExecuteStack();

  void begin();
  void end();

  void addTerm(CAwkOperatorPtr op);
  void addTerm(CAwkExpressionTermPtr term);

  bool checkUnstack(CAwkOperatorPtr op);

  void unstackExpression();

  bool hasLastOp() const;

  double popReal();

  CAwkVariableRefPtr popVariableRef();

  CAwkValuePtr popValue();

  CAwkExpressionTermPtr popTerm();

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkExecuteStack &stack) {
    stack.print(os);

    return os;
  }

 private:
  using ExecuteSubStackStack = std::vector<CAwkExecuteSubStack *>;

  CAwkExecuteSubStack* executeSubStack_      { nullptr };
  ExecuteSubStackStack executeSubStackStack_;
};

//----

class CAwkExecuteSubStack {
 public:
  CAwkExecuteSubStack();

  void addTerm(CAwkOperatorPtr op);
  void addTerm(CAwkExpressionTermPtr term);

  bool checkUnstack(CAwkOperatorPtr op);

  void unstackExpression();

  bool hasLastOp() const { return !!lastOp_; }

  CAwkVariableRefPtr popVariableRef();

  CAwkValuePtr popValue();

  CAwkExpressionTermPtr popTerm();

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkExecuteSubStack &stack) {
    stack.print(os);

    return os;
  }

 private:
  using TermList = CAwkExpressionTermList;
  using OpStack  = CAwkOperatorList;

  TermList        termList_;
  bool            value_ { false };
  OpStack         opStack_;
  CAwkOperatorPtr lastOp_;
};

#endif
