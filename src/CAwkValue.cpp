#include <CAwk.h>
#include <CStrUtil.h>

CAwkValuePtr
CAwkValue::
create(const std::string &value)
{
  return CAwkValuePtr(new CAwkValue(value));
}

CAwkValuePtr
CAwkValue::
create(const char *value)
{
  return CAwkValuePtr(new CAwkValue(value));
}

CAwkValuePtr
CAwkValue::
create(double value)
{
  return CAwkValuePtr(new CAwkValue(value));
}

CAwkValuePtr
CAwkValue::
create(int value)
{
  return CAwkValuePtr(new CAwkValue(value));
}

CAwkValuePtr
CAwkValue::
create(bool value)
{
  return CAwkValuePtr(new CAwkValue(value));
}

//-------------

CAwkValue::
CAwkValue(const std::string &value) :
 value_(value)
{
}

CAwkValue::
CAwkValue(const char *value) :
 value_(value)
{
}

CAwkValue::
CAwkValue(double value) :
 value_(CStrUtil::toString(value))
{
}

CAwkValue::
CAwkValue(int value) :
 value_(CStrUtil::toString(value))
{
}

CAwkValue::
CAwkValue(bool value) :
 value_(value ? "1" : "0")
{
}

bool
CAwkValue::
isReal() const
{
  return CStrUtil::isReal(value_);
}

bool
CAwkValue::
isInteger() const
{
  return CStrUtil::isInteger(value_);
}

bool
CAwkValue::
isBool() const
{
  return (value_ == "1" || value_ == "0");
}

std::string
CAwkValue::
getString() const
{
  return value_;
}

double
CAwkValue::
getReal() const
{
  if      (CStrUtil::isReal(value_))
    return CStrUtil::toReal(value_);
  else if (CStrUtil::isInteger(value_))
    return CStrUtil::toInteger(value_);
  else
    return 0.0;
}

int
CAwkValue::
getInteger() const
{
  if      (CStrUtil::isInteger(value_))
    return CStrUtil::toInteger(value_);
  else if (CStrUtil::isReal(value_))
    return int(CStrUtil::toReal(value_));
  else
    return 0;
}

bool
CAwkValue::
getBool() const
{
  return (value_ != "" && value_ != "0");
}

void
CAwkValue::
setValue(CAwkValuePtr value)
{
  value_ = value->value_;
}

void
CAwkValue::
setString(const std::string &value)
{
  value_ = value;
}

void
CAwkValue::
setReal(double value)
{
  value_ = CStrUtil::toString(value);
}

void
CAwkValue::
setInteger(int value)
{
  value_ = CStrUtil::toString(value);
}

void
CAwkValue::
setBool(bool value)
{
  value_ = (value ? "1" : "0");
}

int
CAwkValue::
cmp(CAwkValuePtr rhs) const
{
  if      ((     isReal() && (rhs->isReal() || rhs->isInteger())) ||
           (rhs->isReal() && (     isReal() ||      isInteger()))) {
    double real1 =      getReal();
    double real2 = rhs->getReal();

    if      (real1 >  real2) return  1;
    else if (real1 <  real2) return -1;
    else                     return  0;
  }
  else if (isInteger() && rhs->isInteger()) {
    double integer1 =      getInteger();
    double integer2 = rhs->getInteger();

    if      (integer1 > integer2) return  1;
    else if (integer1 < integer2) return -1;
    else                          return  0;
  }
  else if (isBool   () && rhs->isBool   ()) {
    bool bool1 =      getBool();
    bool bool2 = rhs->getBool();

    if      (bool1 >  bool2) return  1;
    else if (bool1 <  bool2) return -1;
    else                     return  0;
  }
  else {
    std::string str1 =      getString();
    std::string str2 = rhs->getString();

    if      (str1 >  str2) return  1;
    else if (str1 <  str2) return -1;
    else                   return  0;
  }
}

void
CAwkValue::
print(std::ostream &os) const
{
  if      (isReal())
    os << getReal();
  else if (isInteger())
    os << getInteger();
  else if (isBool())
    os << (getBool() ? "1" : "0");
  else
    os << "\"" << value_ << "\"";
}
