#include <CAwk.h>
#include <CFuncs.h>

CAwkExecuteStack::
CAwkExecuteStack()
{
}

CAwkExecuteStack::
~CAwkExecuteStack()
{
}

void
CAwkExecuteStack::
begin()
{
  executeSubStackStack_.push_back(executeSubStack_);

  executeSubStack_ = new CAwkExecuteSubStack;
}

void
CAwkExecuteStack::
end()
{
  delete executeSubStack_;

  executeSubStack_ = executeSubStackStack_.back();

  executeSubStackStack_.pop_back();
}

double
CAwkExecuteStack::
popReal()
{
  auto value = popValue();

  return value->getReal();
}

CAwkVariableRefPtr
CAwkExecuteStack::
popVariableRef()
{
  return executeSubStack_->popVariableRef();
}

CAwkValuePtr
CAwkExecuteStack::
popValue()
{
  return executeSubStack_->popValue();
}

CAwkExpressionTermPtr
CAwkExecuteStack::
popTerm()
{
  return executeSubStack_->popTerm();
}

void
CAwkExecuteStack::
addTerm(CAwkOperatorPtr op)
{
  executeSubStack_->addTerm(op);
}

void
CAwkExecuteStack::
addTerm(CAwkExpressionTermPtr term)
{
  executeSubStack_->addTerm(term);
}

bool
CAwkExecuteStack::
checkUnstack(CAwkOperatorPtr op)
{
  return executeSubStack_->checkUnstack(op);
}

void
CAwkExecuteStack::
unstackExpression()
{
  executeSubStack_->unstackExpression();
}

bool
CAwkExecuteStack::
hasLastOp() const
{
  return executeSubStack_->hasLastOp();
}

void
CAwkExecuteStack::
print(std::ostream &os) const
{
  executeSubStack_->print(os);
}

//------------------

CAwkExecuteSubStack::
CAwkExecuteSubStack()
{
}

void
CAwkExecuteSubStack::
addTerm(CAwkOperatorPtr op)
{
  if (lastOp_)
    opStack_.push_back(lastOp_);

  termList_.push_back(std::static_pointer_cast<CAwkExpressionTerm>(op));

  lastOp_ = op;
  value_  = false;
}

void
CAwkExecuteSubStack::
addTerm(CAwkExpressionTermPtr term)
{
  if (term->hasValue()) {
    if (value_) {
      auto op = CAwkConcatOperator::create();

      addTerm(op);
    }

    termList_.push_back(std::static_pointer_cast<CAwkExpressionTerm>(term));

    value_ = true;
  }
  else
    assert(false);
}

bool
CAwkExecuteSubStack::
checkUnstack(CAwkOperatorPtr op)
{
  if (lastOp_) {
    if (lastOp_->getPrecedence() > op->getPrecedence() ||
        (lastOp_->getPrecedence() == op->getPrecedence() &&
         op->getDirection() == CAwkOperator::Direction::L_TO_R))
      return true;
  }

  return false;
}

void
CAwkExecuteSubStack::
unstackExpression()
{
  CAwkExpressionTermPtr rterm;

  CAwkOperatorPtr op;

  auto term1 = termList_.back();

  termList_.pop_back();

  auto term2 = termList_.back();

  termList_.pop_back();

  if      (term1->hasValue()) {
    rterm = term1;

    op = std::static_pointer_cast<CAwkOperator>(term2);
  }
  else if (dynamic_cast<CAwkOperator *>(term2.get()) != nullptr) {
    op = std::static_pointer_cast<CAwkOperator>(term2);

    rterm = term1;
  }
  else if (dynamic_cast<CAwkOperator *>(term1.get()) != nullptr) {
    op = std::static_pointer_cast<CAwkOperator>(term1);

    rterm = term2;
  }
  else
    assert(false);

  CAwkExpressionTermPtr result;

  if      (op->isUnary()) {
    termList_.push_back(rterm);

    result = op->execute();

    if (! opStack_.empty()) {
      lastOp_ = opStack_.back();

      opStack_.pop_back();
    }
    else
      lastOp_ = CAwkOperatorPtr();

    if (! termList_.empty()) {
      auto last_term = termList_.back();

      value_ = last_term->hasValue();
    }
    else
      value_ = false;
  }
  else if (op->isBinary()) {
    auto term3 = termList_.back();

    termList_.pop_back();

    auto lterm = term3;

    termList_.push_back(lterm);
    termList_.push_back(rterm);

    result = op->execute();

    if (! opStack_.empty()) {
      lastOp_ = opStack_.back();

      opStack_.pop_back();
    }
    else
      lastOp_ = CAwkOperatorPtr();

    if (! termList_.empty()) {
      auto last_term = termList_.back();

      value_ = last_term->hasValue();
    }
    else
      value_ = false;
  }
  else if (op->isTernary()) {
    // can only be ? or :
    auto term3 = termList_.back();

    termList_.pop_back();

    auto lterm = term3;

    auto lvalue = CAwkInst->getValue(lterm);

    if (dynamic_cast<CAwkQuestionOperator *>(op.get()) != nullptr) {
      bool flag = lvalue->getBool();

      if (flag)
        result = rterm;
      else
        result = CAwkNullValue::create();
    }
    else {
      if (dynamic_cast<CAwkNullValue *>(lvalue.get()))
        result = rterm;
      else
        result = lterm;
    }

    if (! opStack_.empty()) {
      lastOp_ = opStack_.back();

      opStack_.pop_back();
    }
    else
      lastOp_ = CAwkOperatorPtr();

    if (! termList_.empty()) {
      auto last_term = termList_.back();

      value_ = last_term->hasValue();
    }
    else
      value_ = false;
  }
  else
    assert(false);

  addTerm(result);
}

CAwkVariableRefPtr
CAwkExecuteSubStack::
popVariableRef()
{
  auto term = termList_.back();

  termList_.pop_back();

  if (dynamic_cast<CAwkVariableRef *>(term.get()) != nullptr)
    return std::static_pointer_cast<CAwkVariableRef>(term);
  else
    return CAwkVariableRefPtr();
}

CAwkValuePtr
CAwkExecuteSubStack::
popValue()
{
  auto term = popTerm();

  return CAwkInst->getValue(term);
}

CAwkExpressionTermPtr
CAwkExecuteSubStack::
popTerm()
{
  auto term = termList_.back();

  termList_.pop_back();

  return term;
}

void
CAwkExecuteSubStack::
print(std::ostream &os) const
{
  os << "value=\"" << (value_ ? "true" : "false") << "\"";

  if (! termList_.empty()) {
    os << ", terms=\"";

    for_each(termList_.begin(), termList_.end(), CPrintSeparated<CAwkExpressionTermPtr>(os));

    os << "\"";
  }

  if (lastOp_ || ! opStack_.empty()) {
    os << ", ops=\"";

    for_each(opStack_.begin(), opStack_.end(), CPrintSeparated<CAwkOperatorPtr>(os));

    if (lastOp_) {
      if (! opStack_.empty())
        os << " ";

      lastOp_->print(os);
    }

    os << "\"";
  }
}
