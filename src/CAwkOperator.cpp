#include <CAwk.h>
#include <CMathGen.h>

CAwkExpressionTermPtr
CAwkAssignOperator::
execute()
{
  auto value = CAwkInst->getExecuteStack().popValue();

  auto var = CAwkInst->getExecuteStack().popVariableRef();

  if (! var.isValid()) {
    CAwkInst->error("Lhs is not a variable");
    return value.refCast<CAwkExpressionTerm>();
  }

  var->setValue(value);

  return var.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkPlusEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();

  auto var = CAwkInst->getExecuteStack().popVariableRef();

  if (! var.isValid()) {
    CAwkInst->error("Lhs is not a variable");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  CAwkValuePtr value;

  if      (value1->isInteger() && value2->isInteger())
    value = CAwkValue::create(value1->getInteger() + value2->getInteger());
  else if (value1->isReal   () || value2->isReal   ())
    value = CAwkValue::create(value1->getReal   () + value2->getReal   ());
  else
    value = CAwkValue::create("0");

  var->setValue(value);

  return var.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkMinusEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();

  auto var = CAwkInst->getExecuteStack().popVariableRef();

  if (! var.isValid()) {
    CAwkInst->error("Lhs is not a variable");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  CAwkValuePtr value;

  if      (value1->isInteger() && value2->isInteger())
    value = CAwkValue::create(value1->getInteger() - value2->getInteger());
  else if (value1->isReal   () || value2->isReal   ())
    value = CAwkValue::create(value1->getReal   () - value2->getReal   ());
  else
    value = CAwkValue::create("0");

  var->setValue(value);

  return var.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkTimesEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();

  auto var = CAwkInst->getExecuteStack().popVariableRef();

  if (! var.isValid()) {
    CAwkInst->error("Lhs is not a variable");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  CAwkValuePtr value;

  if      (value1->isInteger() && value2->isInteger())
    value = CAwkValue::create(value1->getInteger() * value2->getInteger());
  else if (value1->isReal   () || value2->isReal   ())
    value = CAwkValue::create(value1->getReal   () * value2->getReal   ());
  else
    value = CAwkValue::create("0");

  var->setValue(value);

  return var.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkDivideEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();

  auto var = CAwkInst->getExecuteStack().popVariableRef();

  if (! var.isValid()) {
    CAwkInst->error("Lhs is not a variable");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  CAwkValuePtr value;

  if (value1->isReal() || value2->isReal())
    value = CAwkValue::create(value1->getReal() / value2->getReal());
  else
    value = CAwkValue::create("0");

  var->setValue(value);

  return var.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkModulusEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();

  auto var = CAwkInst->getExecuteStack().popVariableRef();

  if (! var.isValid()) {
    CAwkInst->error("Lhs is not a variable");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  CAwkValuePtr value;

  if      (value1->isInteger() && value2->isInteger())
    value = CAwkValue::create((int)
              CMathGen::modulus((long) value1->getInteger(),
                                (long) value2->getInteger()));
  else if (value1->isReal   () || value2->isReal   ())
    value = CAwkValue::create(
              CMathGen::modulus(value1->getReal   (),
                                value2->getReal   ()));
  else
    value = CAwkValue::create("0");

  var->setValue(value);

  return var.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkPowerEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();

  auto var = CAwkInst->getExecuteStack().popVariableRef();

  if (! var.isValid()) {
    CAwkInst->error("Lhs is not a variable");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  CAwkValuePtr value;

  if      (value1->isInteger() && value2->isInteger())
    value = CAwkValue::create(CMathGen::pow((long) value1->getInteger(),
                                        (long) value2->getInteger()));
  else if (value1->isReal   () || value2->isReal   ())
    value = CAwkValue::create(CMathGen::pow(value1->getReal   (),
                                        value2->getReal   ()));
  else
    value = CAwkValue::create("0");

  var->setValue(value);

  return var.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkQuestionOperator::
execute()
{
  assert(false);

  return CAwkExpressionTermPtr();
}

CAwkExpressionTermPtr
CAwkColonOperator::
execute()
{
  assert(false);

  return CAwkExpressionTermPtr();
}

CAwkExpressionTermPtr
CAwkLogicalOrOperator::
execute()
{
  bool bool2 = CAwkInst->getExecuteStack().popValue()->getBool();
  bool bool1 = CAwkInst->getExecuteStack().popValue()->getBool();

  auto result = CAwkValue::create(bool1 || bool2);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkLogicalAndOperator::
execute()
{
  bool bool2 = CAwkInst->getExecuteStack().popValue()->getBool();
  bool bool1 = CAwkInst->getExecuteStack().popValue()->getBool();

  auto result = CAwkValue::create(bool1 && bool2);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkInOperator::
execute()
{
  auto var = CAwkInst->getExecuteStack().popVariableRef();

  CAwkValuePtr result;

  if (! var.isValid()) {
    CAwkInst->error("value is not a variable");

    result = CAwkValue::create("0");
  }
  else {
    std::string value = CAwkInst->getExecuteStack().popValue()->getString();

    bool flag = var->isInd(value);

    result = CAwkValue::create(flag);
  }

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkRegExpOperator::
execute()
{
  std::string value2 = CAwkInst->getExecuteStack().popValue()->getString();
  std::string value1 = CAwkInst->getExecuteStack().popValue()->getString();

  CRegExp regexp(value2);

  regexp.setExtended(true);

  bool match = regexp.find(value1);

  auto result = CAwkValue::create(match);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkNotRegExpOperator::
execute()
{
  std::string value2 = CAwkInst->getExecuteStack().popValue()->getString();
  std::string value1 = CAwkInst->getExecuteStack().popValue()->getString();

  CRegExp regexp(value2);

  regexp.setExtended(true);

  bool match = regexp.find(value1);

  auto result = CAwkValue::create(! match);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkLessOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  auto result = CAwkValue::create(value1->cmp(value2) < 0);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkLessEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  auto result = CAwkValue::create(value1->cmp(value2) <= 0);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  auto result = CAwkValue::create(value1->cmp(value2) == 0);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkNotEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  auto result = CAwkValue::create(value1->cmp(value2) != 0);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkGreaterEqualsOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  auto result = CAwkValue::create(value1->cmp(value2) >= 0);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkGreaterOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  auto result = CAwkValue::create(value1->cmp(value2) > 0);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkConcatOperator::
execute()
{
  std::string str2 = CAwkInst->getExecuteStack().popValue()->getString();
  std::string str1 = CAwkInst->getExecuteStack().popValue()->getString();

  auto result = CAwkValue::create(str1 + str2);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkPlusOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  CAwkValuePtr result;

  if      (value1->isInteger() && value2->isInteger())
    result = CAwkValue::create(value1->getInteger() + value2->getInteger());
  else if (value1->isReal   () || value2->isReal   ())
    result = CAwkValue::create(value1->getReal   () + value2->getReal   ());
  else
    result = CAwkValue::create("0");

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkMinusOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  CAwkValuePtr result;

  if      (value1->isInteger() && value2->isInteger())
    result = CAwkValue::create(value1->getInteger() - value2->getInteger());
  else if (value1->isReal   () || value2->isReal   ())
    result = CAwkValue::create(value1->getReal   () - value2->getReal   ());
  else
    result = CAwkValue::create("0");

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkTimesOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  CAwkValuePtr result;

  if      (value1->isInteger() && value2->isInteger())
    result = CAwkValue::create(value1->getInteger() * value2->getInteger());
  else if (value1->isReal   () || value2->isReal   ())
    result = CAwkValue::create(value1->getReal   () * value2->getReal   ());
  else
    result = CAwkValue::create("0");

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkDivideOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  CAwkValuePtr result;

  if (value1->isReal() || value2->isReal())
    result = CAwkValue::create(value1->getReal() / value2->getReal());
  else
    result = CAwkValue::create("0");

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkModulusOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  CAwkValuePtr result;

  if      (value1->isInteger() && value2->isInteger())
    result = CAwkValue::create((int)
               CMathGen::modulus((long) value1->getInteger(),
                                 (long) value2->getInteger()));
  else if (value1->isReal   () || value2->isReal   ())
    result = CAwkValue::create(
               CMathGen::modulus(value1->getReal   (),
                                 value2->getReal   ()));
  else
    result = CAwkValue::create("0");

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkUnaryPlusOperator::
execute()
{
  auto value = CAwkInst->getExecuteStack().popValue();

  CAwkValuePtr result;

  if      (value->isInteger())
    result = CAwkValue::create(value->getInteger());
  else if (value->isReal())
    result = CAwkValue::create(value->getReal());
  else
    result = CAwkValue::create("0");

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkUnaryMinusOperator::
execute()
{
  auto value = CAwkInst->getExecuteStack().popValue();

  CAwkValuePtr result;

  if      (value->isInteger())
    result = CAwkValue::create(-value->getInteger());
  else if (value->isReal())
    result = CAwkValue::create(-value->getReal());
  else
    result = CAwkValue::create("0");

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkLogicalNotOperator::
execute()
{
  bool bool1 = CAwkInst->getExecuteStack().popValue()->getBool();

  auto result = CAwkValue::create(! bool1);

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkPowerOperator::
execute()
{
  auto value2 = CAwkInst->getExecuteStack().popValue();
  auto value1 = CAwkInst->getExecuteStack().popValue();

  CAwkValuePtr result;

  if      (value1->isInteger() && value2->isInteger())
    result = CAwkValue::create(CMathGen::pow((long) value1->getInteger(),
                                                  (long) value2->getInteger()));
  else if (value1->isReal   () || value2->isReal   ())
    result = CAwkValue::create(CMathGen::pow(value1->getReal   (),
                                                  value2->getReal   ()));
  else
    result = CAwkValue::create("0");

  return result.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkPreIncrementOperator::
execute()
{
  auto var = CAwkInst->getExecuteStack().popVariableRef();

  CAwkValuePtr value2;

  if (! var.isValid()) {
    CAwkInst->error("value is not a variable");
    value2 = CAwkValue::create("0");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  if      (value1->isInteger())
    value2 = CAwkValue::create(value1->getInteger() + 1);
  else if (value1->isReal   ())
    value2 = CAwkValue::create(value1->getReal   () + 1);
  else
    value2 = CAwkValue::create("1");

  var->setValue(value2);

  return value2.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkPostIncrementOperator::
execute()
{
  auto var = CAwkInst->getExecuteStack().popVariableRef();

  CAwkValuePtr value2;

  if (! var.isValid()) {
    CAwkInst->error("value is not a variable");
    value2 = CAwkValue::create("0");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  if      (value1->isInteger()) {
    value1 = CAwkValue::create(value1->getInteger());
    value2 = CAwkValue::create(value1->getInteger() + 1);
  }
  else if (value1->isReal   ()) {
    value1 = CAwkValue::create(value1->getReal   ());
    value2 = CAwkValue::create(value1->getReal   () + 1);
  }
  else {
    value1 = CAwkValue::create("0");
    value2 = CAwkValue::create("1");
  }

  var->setValue(value2);

  return value1.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkPreDecrementOperator::
execute()
{
  auto var = CAwkInst->getExecuteStack().popVariableRef();

  CAwkValuePtr value2;

  if (! var.isValid()) {
    CAwkInst->error("value is not a variable");
    value2 = CAwkValue::create("0");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  if      (value1->isInteger())
    value2 = CAwkValue::create(value1->getInteger() - 1);
  else if (value1->isReal   ())
    value2 = CAwkValue::create(value1->getReal   () - 1);
  else
    value2 = CAwkValue::create("-1");

  var->setValue(value2);

  return value2.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkPostDecrementOperator::
execute()
{
  auto var = CAwkInst->getExecuteStack().popVariableRef();

  CAwkValuePtr value2;

  if (! var.isValid()) {
    CAwkInst->error("value is not a variable");
    value2 = CAwkValue::create("0");
    return value2.refCast<CAwkExpressionTerm>();
  }

  auto value1 = var->getValue();

  if      (value1->isInteger()) {
    value1 = CAwkValue::create(value1->getInteger());
    value2 = CAwkValue::create(value1->getInteger() - 1);
  }
  else if (value1->isReal   ()) {
    value1 = CAwkValue::create(value1->getReal   ());
    value2 = CAwkValue::create(value1->getReal   () - 1);
  }
  else {
    value1 = CAwkValue::create("0");
    value2 = CAwkValue::create("-1");
  }

  var->setValue(value2);

  return value1.refCast<CAwkExpressionTerm>();
}

CAwkExpressionTermPtr
CAwkFieldOperator::
execute()
{
  auto value = CAwkInst->getExecuteStack().popValue();

  CAwkVariableRefPtr result;

  if (value->isInteger()) {
    int pos = value->getInteger();

    result = CAwkFieldVariableRef::create(pos);
  }
  else
    result = CAwkVariableRef::create(value->getString());

  return result.refCast<CAwkExpressionTerm>();
}
