#ifndef CAWK_EXECUTE_STACK_H
#define CAWK_EXECUTE_STACK_H

#include <CAwkTypes.h>

class CAwkExecuteSubStack;
class CAwkVariableRef;

class CAwkExecuteStack {
 private:
  typedef std::vector<CAwkExecuteSubStack *> ExecuteSubStackStack;

  CAwkExecuteSubStack  *executeSubStack_;
  ExecuteSubStackStack  executeSubStackStack_;

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
};

class CAwkExecuteSubStack {
 private:
  typedef CAwkExpressionTermList TermList;
  typedef CAwkOperatorList       OpStack;

  TermList        termList_;
  bool            value_;
  OpStack         opStack_;
  CAwkOperatorPtr lastOp_;

 public:
  CAwkExecuteSubStack();

  void addTerm(CAwkOperatorPtr op);
  void addTerm(CAwkExpressionTermPtr term);

  bool checkUnstack(CAwkOperatorPtr op);

  void unstackExpression();

  bool hasLastOp() const { return lastOp_.isValid(); }

  CAwkVariableRefPtr popVariableRef();

  CAwkValuePtr popValue();

  CAwkExpressionTermPtr popTerm();

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkExecuteSubStack &stack) {
    stack.print(os);

    return os;
  }
};

#endif
