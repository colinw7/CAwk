#include <CAwk.h>
#include <CFuncs.h>

CAwkExecuteStack::
CAwkExecuteStack() :
 executeSubStack_(0)
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
  CAwkValuePtr value = popValue();

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
CAwkExecuteSubStack() :
 value_(false)
{
}

void
CAwkExecuteSubStack::
addTerm(CAwkOperatorPtr op)
{
  if (lastOp_.isValid())
    opStack_.push_back(lastOp_);

  termList_.push_back(op.refCast<CAwkExpressionTerm>());

  lastOp_ = op;
  value_  = false;
}

void
CAwkExecuteSubStack::
addTerm(CAwkExpressionTermPtr term)
{
  if (term->hasValue()) {
    if (value_) {
      CAwkOperatorPtr op = CAwkConcatOperator::create();

      addTerm(op);
    }

    termList_.push_back(term.refCast<CAwkExpressionTerm>());

    value_ = true;
  }
  else
    assert(false);
}

bool
CAwkExecuteSubStack::
checkUnstack(CAwkOperatorPtr op)
{
  if (lastOp_.isValid()) {
    if (lastOp_->getPrecedence() > op->getPrecedence() ||
        (lastOp_->getPrecedence() == op->getPrecedence() &&
         op->getDirection() == CAwkOperator::EVAL_L_TO_R))
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

  CAwkExpressionTermPtr term1 = termList_.back();

  termList_.pop_back();

  CAwkExpressionTermPtr term2 = termList_.back();

  termList_.pop_back();

  if      (term1->hasValue()) {
    rterm = term1;

    op = term2.refCast<CAwkOperator>();
  }
  else if (term2.canCast<CAwkOperator>()) {
    op = term2.refCast<CAwkOperator>();

    rterm = term1;
  }
  else if (term1.canCast<CAwkOperator>()) {
    op = term1.refCast<CAwkOperator>();

    rterm = term2;
  }
  else
    assert(false);

  CAwkExpressionTermPtr result;

  if      (op->getType() & CAwkOperator::OP_TYPE_UNARY) {
    termList_.push_back(rterm);

    result = op->execute();

    if (! opStack_.empty()) {
      lastOp_ = opStack_.back();

      opStack_.pop_back();
    }
    else
      lastOp_ = CAwkOperatorPtr();

    if (! termList_.empty()) {
      CAwkExpressionTermPtr last_term = termList_.back();

      value_ = last_term->hasValue();
    }
    else
      value_ = false;
  }
  else if (op->getType() & CAwkOperator::OP_TYPE_BINARY) {
    CAwkExpressionTermPtr term3 = termList_.back();

    termList_.pop_back();

    CAwkExpressionTermPtr lterm = term3;

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
      CAwkExpressionTermPtr last_term = termList_.back();

      value_ = last_term->hasValue();
    }
    else
      value_ = false;
  }
  else if (op->getType() & CAwkOperator::OP_TYPE_TERNARY) {
    // can only be ? or :
    CAwkExpressionTermPtr term3 = termList_.back();

    termList_.pop_back();

    CAwkExpressionTermPtr lterm = term3;

    CAwkValuePtr lvalue = CAwkInst->getValue(lterm);

    if (op.cast<CAwkQuestionOperator>() != 0) {
      bool flag = lvalue->getBool();

      if (flag)
        result = rterm;
      else
        result = CAwkNullValue::create();
    }
    else {
      if (lvalue.cast<CAwkNullValue>())
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
      CAwkExpressionTermPtr last_term = termList_.back();

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
  CAwkExpressionTermPtr term = termList_.back();

  termList_.pop_back();

  if (term.canCast<CAwkVariableRef>())
    return term.refCast<CAwkVariableRef>();
  else
    return CAwkVariableRefPtr();
}

CAwkValuePtr
CAwkExecuteSubStack::
popValue()
{
  CAwkExpressionTermPtr term = popTerm();

  return CAwkInst->getValue(term);
}

CAwkExpressionTermPtr
CAwkExecuteSubStack::
popTerm()
{
  CAwkExpressionTermPtr term = termList_.back();

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

  if (lastOp_.isValid() || ! opStack_.empty()) {
    os << ", ops=\"";

    for_each(opStack_.begin(), opStack_.end(), CPrintSeparated<CAwkOperatorPtr>(os));

    if (lastOp_.isValid()) {
      if (! opStack_.empty())
        os << " ";

      lastOp_->print(os);
    }

    os << "\"";
  }
}
