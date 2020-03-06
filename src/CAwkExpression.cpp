#include <CAwk.h>
#include <CFuncs.h>
#include <CReadLine.h>

CAwkExpression::
CAwkExpression() :
 value_(false)
{
}

CAwkValuePtr
CAwkExpression::
getValue() const
{
  CAwkExpression *th = const_cast<CAwkExpression *>(this);

  CAwkExpressionTermPtr term = th->execute();

  return CAwkInst->getValue(term);
}

void
CAwkExpression::
pushTerm(CAwkExpressionTermPtr term)
{
  if      (term.canCast<CAwkOperator>()) {
    CAwkOperatorPtr op = term.refCast<CAwkOperator>();

    if (value_ && op->isUnary()) {
      if ((op.cast<CAwkPostIncrementOperator>() == 0) &&
          (op.cast<CAwkPostDecrementOperator>() == 0)) {
        CAwkOperatorPtr op1 = CAwkConcatOperator::create();

        pushTerm(op1.refCast<CAwkExpressionTerm>());
      }
    }

    termList_.push_back(term);

    if (lastOp_.isValid())
      opStack_.push_back(lastOp_);

    lastOp_ = op;
    value_  = false;
  }
  else if (term->hasValue()) {
    if (value_) {
      CAwkOperatorPtr op = CAwkConcatOperator::create();

      pushTerm(op.refCast<CAwkExpressionTerm>());
    }

    termList_.push_back(term);

    value_ = true;
  }
  else
    assert(false);
}

CAwkExpressionTermPtr
CAwkExpression::
execute()
{
  CAwkExecuteStack &executeStack = CAwkInst->getExecuteStack();

  executeStack.begin();

  if (CAwkInst->getDebug())
    std::cout << executeStack << std::endl;

  TermList::iterator p1 = termList_.begin();
  TermList::iterator p2 = termList_.end  ();

  for ( ; p1 != p2; ++p1) {
    CAwkExpressionTermPtr term = *p1;

    if      (term.canCast<CAwkOperator>()) {
      CAwkOperatorPtr op = term.refCast<CAwkOperator>();

      while (executeStack.checkUnstack(op)) {
        executeStack.unstackExpression();

        if (CAwkInst->getDebug())
          std::cout << executeStack << std::endl;
      }

      executeStack.addTerm(op);
    }
    else if (term.canCast<CAwkExprFunction>()) {
      CAwkExprFunctionPtr func = term.refCast<CAwkExprFunction>();

      CAwkExpressionTermPtr term1 = func->execute();

      if (term1.isValid())
        executeStack.addTerm(term1);
      else {
        CAwkValuePtr result = CAwkValue::create("");

        executeStack.addTerm(result.refCast<CAwkExpressionTerm>());
      }
    }
    else if (term->hasValue())
      executeStack.addTerm(term);
    else
      assert(false);

    if (CAwkInst->getDebug())
      std::cout << executeStack << std::endl;
  }

  while (executeStack.hasLastOp()) {
    executeStack.unstackExpression();

    if (CAwkInst->getDebug())
      std::cout << executeStack << std::endl;
   }

  CAwkExpressionTermPtr term = executeStack.popTerm();

  executeStack.end();

  return term;
}

void
CAwkExpression::
print(std::ostream &os) const
{
  os << "(";

  for_each(termList_.begin(), termList_.end(),
           CPrintSeparated<CAwkExpressionTermPtr>(os));

  os << ")";
}

//-------------

CAwkValuePtr
CAwkGetLineExpr::
getValue() const
{
  std::string line;

  if (file_.isValid()) {
    std::string str;

    file_->read(line);
  }
  else {
    CReadLine readline;

    line = readline.readLine();
  }

  return CAwkValue::create(line);
}

CAwkExpressionTermPtr
CAwkGetLineExpr::
execute()
{
  return getValue().refCast<CAwkExpressionTerm>();
}

void
CAwkGetLineExpr::
print(std::ostream &os) const
{
  os << "getline";

  if (var_.isValid())
    os << " " << *var_;

  if (file_.isValid())
    os << " " << *file_;
}
