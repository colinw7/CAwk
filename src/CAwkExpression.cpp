#include <CAwk.h>
#include <CFuncs.h>
#include <CReadLine.h>

CAwkExpression::
CAwkExpression()
{
}

CAwkExpression::
~CAwkExpression()
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
  if      (dynamic_cast<CAwkOperator *>(term.get()) != nullptr) {
    auto op = std::static_pointer_cast<CAwkOperator>(term);

    if (value_ && op->isUnary()) {
      if ((dynamic_cast<CAwkPostIncrementOperator *>(op.get()) == nullptr) &&
          (dynamic_cast<CAwkPostDecrementOperator *>(op.get()) == nullptr)) {
        auto op1 = CAwkConcatOperator::create();

        pushTerm(std::static_pointer_cast<CAwkExpressionTerm>(op1));
      }
    }

    termList_.push_back(term);

    if (lastOp_)
      opStack_.push_back(lastOp_);

    lastOp_ = op;
    value_  = false;
  }
  else if (term->hasValue()) {
    if (value_) {
      auto op = CAwkConcatOperator::create();

      pushTerm(std::static_pointer_cast<CAwkExpressionTerm>(op));
    }

    termList_.push_back(term);

    value_ = true;
  }
  else if (dynamic_cast<CAwkGetLineExpr *>(term.get()) != nullptr) {
    auto expr = std::static_pointer_cast<CAwkGetLineExpr>(term);

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

    if      (dynamic_cast<CAwkOperator *>(term.get()) != nullptr) {
      auto op = std::static_pointer_cast<CAwkOperator>(term);

      while (executeStack.checkUnstack(op)) {
        executeStack.unstackExpression();

        if (awk->getDebug())
          std::cout << executeStack << std::endl;
      }

      executeStack.addTerm(op);
    }
    else if (dynamic_cast<CAwkExprFunction *>(term.get()) != nullptr) {
      auto func = std::static_pointer_cast<CAwkExprFunction>(term);

      auto term1 = func->execute();

      if (term1)
        executeStack.addTerm(term1);
      else {
        auto result = CAwkValue::create("");

        executeStack.addTerm(std::static_pointer_cast<CAwkExpressionTerm>(result));
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

  if (file_) {
    std::string str;

    file_->read(line);
  }
  else {
    CReadLine readline;

    line = readline.readLine();
  }

  auto value = CAwkValue::create(line);

  if (var_) {
    var_->setValue(value);
  }

  return value;
}

CAwkExpressionTermPtr
CAwkGetLineExpr::
execute()
{
  return std::static_pointer_cast<CAwkExpressionTerm>(getValue());
}

void
CAwkGetLineExpr::
print(std::ostream &os) const
{
  os << "getline";

  if (var_)
    os << " " << *var_;

  if (file_)
    os << " " << *file_;
}
