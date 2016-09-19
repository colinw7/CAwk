#include <CAwk.h>
#include <CFuncs.h>
#include <CPrintF.h>
#include <CMathRand.h>
#include <CStrUtil.h>
#include <CRegExp.h>
#include <cmath>

void
CAwkFunctionMgr::
clear()
{
  functionMap_.clear();
}

void
CAwkFunctionMgr::
addFunction(CAwkFunctionPtr function)
{
  functionMap_[function->getName()] = function;
}

CAwkFunctionPtr
CAwkFunctionMgr::
getFunction(const std::string &name) const
{
  FunctionMap::const_iterator p = functionMap_.find(name);

  if (p != functionMap_.end())
    return p->second;

  return CAwkFunctionPtr();
}

void
CAwkFunctionMgr::
print(std::ostream &os) const
{
  CPrintSeparated<CAwkFunction> ps(os);

  for (const auto &f : functionMap_)
    ps(f.second.get());
}

//-------------

CAwkValuePtr
CAwkParseFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() > args_.size()) {
    CAwkInst->error("Too many function args");
    return CAwkValue::create("");
  }

  CAwkActionBlockPtr block = CAwkActionBlock::create(actionList_);

  CAwkInst->startBlock(block);

  // assign values to args
  StringVectorT::const_iterator pa1 = args_.begin();
  StringVectorT::const_iterator pa2 = args_.end();

  CAwkExpressionTermList::const_iterator pv1 = values.begin();
  CAwkExpressionTermList::const_iterator pv2 = values.end  ();

  for ( ; pa1 != pa2 && pv1 != pv2; ++pa1, ++pv1)
    CAwkInst->addVariable(*pa1)->setValue((*pv1)->getValue());

  // these extra arguments are for local variables
  for ( ; pa1 != pa2; ++pa1)
    CAwkInst->addVariable(*pa1)->setValue(CAwkValue::create(""));

  // TODO: new variables are created in global scope !!

  // execute block
  block->exec();

  CAwkValuePtr retValue = CAwkInst->getReturnValue();

  // TODO: old variable list
  CAwkInst->endBlock();

  return retValue;
}

void
CAwkParseFunction::
print(std::ostream &os) const
{
  os << "function " << name_ << "(";

  for_each(args_.begin(), args_.end(), CPrintSeparated<std::string>(os));

  os << ") ";

  actionList_->print(os);
}

//-------------

CAwkValuePtr
CAwkExprFunction::
getValue() const
{
  CAwkFunctionPtr function = CAwkInst->getFunction(name_);

  if (! function.isValid()) {
    CAwkInst->error("No function '" + name_ + "'");
    return CAwkValue::create("");
  }

  CAwkExpressionTermList values;

  CAwkExpressionList::const_iterator p1 = expressionList_.begin();
  CAwkExpressionList::const_iterator p2 = expressionList_.end  ();

  for ( ; p1 != p2; ++p1)
    values.push_back((*p1).refCast<CAwkExpressionTerm>());

  return function->exec(values);
}

CAwkExpressionTermPtr
CAwkExprFunction::
execute()
{
  return getValue().refCast<CAwkExpressionTerm>();
}

void
CAwkExprFunction::
print(std::ostream &os) const
{
  os << name_ << "(";

  for_each(expressionList_.begin(), expressionList_.end(),
           CPrintSeparated<CAwkExpressionPtr>(os, ","));

  os << ")";
}

//-------------

CAwkValuePtr
CAwkGsubFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 2 && values.size() != 3) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  std::string regstr = values[0]->getValue()->getString();

  CRegExp regexp(regstr);

  regexp.setExtended(true);

  std::string rstr = values[1]->getValue()->getString();

  int count;

  std::string str, ostr;

  if (values.size() == 3) {
    CAwkVariableRefPtr var = CAwkInst->getVariableRef(values[2]);

    if (! var.isValid()) {
      CAwkInst->error("Invalid LHS");
      return CAwkValue::create(0);
    }

    str = var->getValue()->getString();

    count = CRegExpUtil::gregsub(str, regexp, rstr, ostr);

    var->setValue(CAwkValue::create(ostr));
  }
  else {
    str = CAwkInst->getLineField(0);

    count = CRegExpUtil::gregsub(str, regexp, rstr, ostr);

    CAwkInst->setLineField(0, ostr);
  }

  return CAwkValue::create(count);
}

void
CAwkGsubFunction::
print(std::ostream &os) const
{
  os << "gsub()";
}

CAwkValuePtr
CAwkIndexFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 2) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  std::string str1 = values[0]->getValue()->getString();
  std::string str2 = values[1]->getValue()->getString();

  int pos = str1.find(str2);

  return CAwkValue::create(pos + 1);
}

void
CAwkIndexFunction::
print(std::ostream &os) const
{
  os << "index()";
}

CAwkValuePtr
CAwkLengthFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  std::string str = values[0]->getValue()->getString();

  int result = str.size();

  return CAwkValue::create(result);
}

void
CAwkLengthFunction::
print(std::ostream &os) const
{
  os << "length()";
}

CAwkValuePtr
CAwkMatchFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 2) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  std::string str1 = values[0]->getValue()->getString();
  std::string str2 = values[1]->getValue()->getString();

  CRegExp regexp(str2);

  regexp.setExtended(true);

  bool match = regexp.find(str1);

  if (match) {
    int start, end;

    regexp.getMatchRange(&start, &end);

    CAwkInst->getVariable("RSTART" )->
      setValue(CAwkValue::create(start + 1));
    CAwkInst->getVariable("RLENGTH")->
      setValue(CAwkValue::create(end - start + 1));

    return CAwkValue::create(start + 1);
  }
  else
    return CAwkValue::create("");
}

void
CAwkMatchFunction::
print(std::ostream &os) const
{
  os << "match()";
}

CAwkValuePtr
CAwkSplitFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 2 && values.size() != 3) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  std::string str = values[0]->getValue()->getString();

  CAwkVariableRefPtr var = CAwkInst->getVariableRef(values[1]);

  if (! var.isValid()) {
    CAwkInst->error("Invalid LHS");
    return CAwkValue::create(0);
  }

  std::string fs;

  if (values.size() == 3)
    fs = values[2]->getValue()->getString();
  else
    fs = CAwkInst->getVariable("FS")->getValue()->getString();

  if (fs == " ")
    fs = " \t";

  StringVectorT fields;

  CStrUtil::addFields(str, fields, fs, /*skipEmpty*/true);

  uint numFields = fields.size();

  for (uint i = 0; i < numFields; ++i)
    var->setIndValue(CStrUtil::toString(i + 1), CAwkValue::create(fields[i]));

  return CAwkValue::create((int) numFields);
}

void
CAwkSplitFunction::
print(std::ostream &os) const
{
  os << "split()";
}

CAwkValuePtr
CAwkSprintfFunction::
exec(const CAwkExpressionTermList &values)
{
  class PrintF : public CPrintF {
   private:
    const CAwkExpressionTermList &values_;
    mutable uint                        pos_;

   public:
    PrintF(const std::string &format, const CAwkExpressionTermList &values) :
     CPrintF(format), values_(values), pos_(1) {
    }

    int         getInt     () const { return getValue()->getInteger(); }
    long        getLong    () const { return getValue()->getInteger(); }
    long        getLongLong() const { return getValue()->getInteger(); }
    double      getDouble  () const { return getValue()->getReal   (); }
    std::string getString  () const { return getValue()->getString (); }

    CAwkValuePtr getValue() const {
      CAwkValuePtr value;

      if (pos_ < values_.size()) {
        value = values_[pos_]->getValue();

        ++pos_;
      }

      if (! value.isValid())
        value = CAwkValue::create("");

      return value;
    }
  };

  if (values.size() < 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  std::string format = values[0]->getValue()->getString();

  PrintF printf(format, values);

  std::string str = printf.format();

  return CAwkValue::create(str);
}

void
CAwkSprintfFunction::
print(std::ostream &os) const
{
  os << "sprintf()";
}

CAwkValuePtr
CAwkSubFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 2 && values.size() != 3) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  std::string regstr = values[0]->getValue()->getString();

  CRegExp regexp(regstr);

  regexp.setExtended(true);

  std::string rstr = values[1]->getValue()->getString();

  int count = 0;

  std::string str, ostr;

  if (values.size() == 3) {
    CAwkVariableRefPtr var = CAwkInst->getVariableRef(values[2]);

    if (! var.isValid()) {
      CAwkInst->error("Invalid LHS");
      return CAwkValue::create(0);
    }

    str = var->getValue()->getString();

    if (CRegExpUtil::regsub(str, regexp, rstr, ostr))
      ++count;

    var->setValue(CAwkValue::create(ostr));
  }
  else {
    str = CAwkInst->getLineField(0);

    if (CRegExpUtil::regsub(str, regexp, rstr, ostr))
      ++count;

    CAwkInst->setLineField(0, ostr);
  }

  return CAwkValue::create(count);
}

void
CAwkSubFunction::
print(std::ostream &os) const
{
  os << "sub()";
}

CAwkValuePtr
CAwkSubstrFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 2 && values.size() != 3) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  std::string str = values[0]->getValue()->getString ();
  int         pos = values[1]->getValue()->getInteger();

  if (values.size() == 3) {
    int len = values[2]->getValue()->getInteger();

    return CAwkValue::create(str.substr(pos - 1, len));
  }
  else
    return CAwkValue::create(str.substr(pos - 1));

  return CAwkValuePtr();
}

void
CAwkSubstrFunction::
print(std::ostream &os) const
{
  os << "substr()";
}

CAwkValuePtr
CAwkAtan2Function::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 2) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  double value1 = values[0]->getValue()->getReal();
  double value2 = values[1]->getValue()->getReal();

  double result = atan2(value1, value2);

  return CAwkValue::create(result);
}

void
CAwkAtan2Function::
print(std::ostream &os) const
{
  os << "atan2()";
}

CAwkValuePtr
CAwkCosFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  double value = values[0]->getValue()->getReal();

  double result = cos(value);

  return CAwkValue::create(result);
}

void
CAwkCosFunction::
print(std::ostream &os) const
{
  os << "cos()";
}

CAwkValuePtr
CAwkExpFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  double value = values[0]->getValue()->getReal();

  double result = exp(value);

  return CAwkValue::create(result);
}

void
CAwkExpFunction::
print(std::ostream &os) const
{
  os << "exp()";
}

CAwkValuePtr
CAwkIntFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  double value = values[0]->getValue()->getReal();

  double result = int(value);

  return CAwkValue::create(result);
}

void
CAwkIntFunction::
print(std::ostream &os) const
{
  os << "int()";
}

CAwkValuePtr
CAwkLogFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  double value = values[0]->getValue()->getReal();

  double result = log(value);

  return CAwkValue::create(result);
}

void
CAwkLogFunction::
print(std::ostream &os) const
{
  os << "log()";
}

CAwkValuePtr
CAwkRandFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 0) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  double result = CMathRand::unitRand();

  return CAwkValue::create(result);
}

void
CAwkRandFunction::
print(std::ostream &os) const
{
  os << "rand()";
}

CAwkValuePtr
CAwkSinFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  double value = values[0]->getValue()->getReal();

  double result = sin(value);

  return CAwkValue::create(result);
}

void
CAwkSinFunction::
print(std::ostream &os) const
{
  os << "sin()";
}

CAwkValuePtr
CAwkSqrtFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  double value = values[0]->getValue()->getReal();

  double result = sqrt(value);

  return CAwkValue::create(result);
}

void
CAwkSqrtFunction::
print(std::ostream &os) const
{
  os << "sqrt()";
}

CAwkValuePtr
CAwkSrandFunction::
exec(const CAwkExpressionTermList &values)
{
  if (values.size() != 0 && values.size() != 1) {
    CAwkInst->error("Invalid number of arguments");
    return CAwkValue::create("");
  }

  if (values.size() == 1) {
    double value = values[0]->getValue()->getReal();

    srand(int(value));
  }
  else
    srand(time(0));

  return CAwkValue::create("");
}

void
CAwkSrandFunction::
print(std::ostream &os) const
{
  os << "srand()";
}
