#include <CAwk.h>
#include <CFuncs.h>
#include <CReadLine.h>

CAwkExpression::
CAwkExpression()
{
}

CAwkValuePtr
CAwkExpression::
getValue() const
{
  auto *th = const_cast<CAwkExpression *>(this);

  auto term = th->execute();

  return CAwkInst->getValue(term);
}

void
CAwkExpression::
pushTerm(CAwkExpressionTermPtr term)
{
  if      (term.canCast<CAwkOperator>()) {
    auto op = term.refCast<CAwkOperator>();

    if (value_ && op->isUnary()) {
      if ((op.cast<CAwkPostIncrementOperator>() == nullptr) &&
          (op.cast<CAwkPostDecrementOperator>() == nullptr)) {
        auto op1 = CAwkConcatOperator::create();

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
      auto op = CAwkConcatOperator::create();

      pushTerm(op.refCast<CAwkExpressionTerm>());
    }

    termList_.push_back(term);

    value_ = true;
  }
  else if (term.canCast<CAwkGetLineExpr>()) {
    auto expr = term.refCast<CAwkGetLineExpr>();

    auto term1 = expr->execute();

    termList_.push_back(term1);

    value_ = true;
  }
  else
    assert(false);
}

CAwkExpressionTermPtr
CAwkExpression::
execute()
{
  auto *awk = CAwkInst;

  auto &executeStack = awk->getExecuteStack();

  executeStack.begin();

  if (awk->getDebug())
    std::cout << executeStack << std::endl;

  auto p1 = termList_.begin();
  auto p2 = termList_.end  ();

  for ( ; p1 != p2; ++p1) {
    auto term = *p1;

    if      (term.canCast<CAwkOperator>()) {
      auto op = term.refCast<CAwkOperator>();

      while (executeStack.checkUnstack(op)) {
        executeStack.unstackExpression();

        if (awk->getDebug())
          std::cout << executeStack << std::endl;
      }

      executeStack.addTerm(op);
    }
    else if (term.canCast<CAwkExprFunction>()) {
      auto func = term.refCast<CAwkExprFunction>();

      auto term1 = func->execute();

      if (term1.isValid())
        executeStack.addTerm(term1);
      else {
        auto result = CAwkValue::create("");

        executeStack.addTerm(result.refCast<CAwkExpressionTerm>());
      }
    }
    else if (term->hasValue()) {
      executeStack.addTerm(term);
    }
    else
      assert(false);

    if (awk->getDebug())
      std::cout << executeStack << std::endl;
  }

  while (executeStack.hasLastOp()) {
    executeStack.unstackExpression();

    if (awk->getDebug())
      std::cout << executeStack << std::endl;
   }

  auto term = executeStack.popTerm();

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

  auto value = CAwkValue::create(line);

  if (var_.isValid()) {
    var_->setValue(value);
  }

  return value;
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
