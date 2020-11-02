#include <CAwk.h>
#include <CFuncs.h>

void
CAwkVariableMgr::
addVariable(const std::string &name, const std::string &value)
{
  addVariable(CAwkVariable::create(name, value));
}

void
CAwkVariableMgr::
addVariable(CAwkVariablePtr var)
{
  variableMap_[var->getName()] = var;
}

CAwkVariablePtr
CAwkVariableMgr::
getVariable(const std::string &name) const
{
  auto p = variableMap_.find(name);

  if (p != variableMap_.end())
    return p->second;

  return CAwkVariablePtr();
}

void
CAwkVariableMgr::
print(std::ostream &os) const
{
  CPrintSeparated<CAwkVariable> ps(os);

  for (const auto &v : variableMap_)
    ps(v.second.get());
}

//-------------

CAwkVariablePtr
CAwkVariable::
create(const std::string &name, const std::string &value)
{
  return CAwkVariablePtr(new CAwkVariable(name, value));
}

CAwkVariablePtr
CAwkVariable::
create(const std::string &name, const char *value)
{
  return CAwkVariablePtr(new CAwkVariable(name, value));
}

CAwkVariablePtr
CAwkVariable::
create(const std::string &name, double value)
{
  return CAwkVariablePtr(new CAwkVariable(name, value));
}

CAwkVariablePtr
CAwkVariable::
create(const std::string &name, int value)
{
  return CAwkVariablePtr(new CAwkVariable(name, value));
}

CAwkVariablePtr
CAwkVariable::
create(const std::string &name, bool value)
{
  return CAwkVariablePtr(new CAwkVariable(name, value));
}

//-------------

CAwkVariable::
CAwkVariable(const std::string &name, const char *value) :
 name_(name)
{
  value_ = CAwkValue::create(value);
}

CAwkVariable::
CAwkVariable(const std::string &name, const std::string &value) :
 name_(name)
{
  value_ = CAwkValue::create(value);
}

CAwkVariable::
CAwkVariable(const std::string &name, double value) :
 name_(name)
{
  value_ = CAwkValue::create(value);
}

CAwkVariable::
CAwkVariable(const std::string &name, int value) :
 name_(name)
{
  value_ = CAwkValue::create(value);
}

CAwkVariable::
CAwkVariable(const std::string &name, bool value) :
 name_(name)
{
  value_ = CAwkValue::create(value);
}

CAwkValuePtr
CAwkVariable::
getValue() const
{
  return value_;
}

CAwkValuePtr
CAwkVariable::
getIndValue(const std::string &ind) const
{
  auto p = indValueMap_.find(ind);

  if (p != indValueMap_.end())
    return p->second;

  auto value = CAwkValue::create("");

  auto *th = const_cast<CAwkVariable *>(this);

  th->indValueMap_[ind] = value;

  return value;
}

void
CAwkVariable::
setValue(CAwkValuePtr value)
{
  value_ = value;

  value_.uniquify();
}

void
CAwkVariable::
setIndValue(const std::string &ind, CAwkValuePtr value)
{
  auto p = indValueMap_.find(ind);

  if (p != indValueMap_.end()) {
    p->second = value;
    return;
  }

  indValueMap_[ind] = value;
}

bool
CAwkVariable::
isInd(const std::string &ind) const
{
  auto p = indValueMap_.find(ind);

  return (p != indValueMap_.end());
}

void
CAwkVariable::
removeInd(const std::string &ind)
{
  auto p = indValueMap_.find(ind);

  indValueMap_.erase(p);
}

StringVectorT
CAwkVariable::
getIndices() const
{
  StringVectorT indices;

  for (const auto &pi : indValueMap_)
    indices.push_back(pi.first);

  //std::sort(indices.begin(), indices.end());

  return indices;
}

CAwkExpressionTermPtr
CAwkVariable::
execute()
{
  return getValue().refCast<CAwkExpressionTerm>();
}

void
CAwkVariable::
print(std::ostream &os) const
{
  os << name_ << "=" << *value_;
}

//-----------

void
CAwkVariableRef::
instantiate(bool global)
{
  (void) CAwkInst->getVariable(name_, /*create*/true, global);
}

CAwkValuePtr
CAwkVariableRef::
getValue() const
{
  return CAwkInst->getVariable(name_, true)->getValue();
}

void
CAwkVariableRef::
setValue(CAwkValuePtr value)
{
  CAwkInst->getVariable(name_, true)->setValue(value);
}

bool
CAwkVariableRef::
isInd(const std::string &ind) const
{
  // TODO: create ?
  return CAwkInst->getVariable(name_, true)->isInd(ind);
}

void
CAwkVariableRef::
removeInd(const std::string &ind)
{
  // TODO: create ?
  return CAwkInst->getVariable(name_, true)->removeInd(ind);
}

CAwkValuePtr
CAwkVariableRef::
getIndValue(const std::string &ind) const
{
  // TODO: create ?
  return CAwkInst->getVariable(name_, true)->getIndValue(ind);
}

void
CAwkVariableRef::
setIndValue(const std::string &ind, CAwkValuePtr value)
{
  CAwkInst->getVariable(name_, true)->setIndValue(ind, value);
}

StringVectorT
CAwkVariableRef::
getIndices() const
{
  // TODO: create ?
  return CAwkInst->getVariable(name_, true)->getIndices();
}

CAwkExpressionTermPtr
CAwkVariableRef::
execute()
{
  return getValue().refCast<CAwkExpressionTerm>();
}

void
CAwkVariableRef::
print(std::ostream &os) const
{
  os << "&" << name_;
}

//-----------

CAwkValuePtr
CAwkArrayVariableRef::
getValue() const
{
  std::string ind = getInd();

  return CAwkVariableRef::getIndValue(ind);
}

void
CAwkArrayVariableRef::
setValue(CAwkValuePtr value)
{
  std::string ind = getInd();

  CAwkVariableRef::setIndValue(ind, value);
}

CAwkExpressionTermPtr
CAwkArrayVariableRef::
execute()
{
  return getValue().refCast<CAwkExpressionTerm>();
}

void
CAwkArrayVariableRef::
print(std::ostream &os) const
{
  CAwkVariableRef::print(os);

  os << "[";

  int i = 0;

  for (auto &expr : expressionList_) {
    if (i > 0)
      os << ",";

    os << *expr;

    ++i;
  }

  os << "]";
}

std::string
CAwkArrayVariableRef::
getInd() const
{
  std::string ind;

  int i = 0;

  for (auto &expr : expressionList_) {
    if (i > 0)
      ind += CAwkInst->getVariable("SUBSEP")->getValue()->getString();

    ind += expr->getValue()->getString();

    ++i;
  }

  return ind;
}

//-----------

CAwkValuePtr
CAwkFieldVariableRef::
getValue() const
{
  return CAwkValue::create(CAwkInst->getLineField(pos_));
}

void
CAwkFieldVariableRef::
setValue(CAwkValuePtr value)
{
  CAwkInst->setLineField(pos_, value->getString());
}

void
CAwkFieldVariableRef::
print(std::ostream &os) const
{
  os << "$" << pos_;
}

CAwkExpressionTermPtr
CAwkFieldVariableRef::
execute()
{
  return getValue().refCast<CAwkExpressionTerm>();
}
