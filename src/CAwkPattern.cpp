#include <CAwk.h>

bool
CAwkRegExpPattern::
exec()
{
  std::string str = CAwkInst->getLineField(0);

  return regexp_.find(str);
}

void
CAwkRegExpPattern::
print(std::ostream &os) const
{
  os << "/" << regexp_.getPattern() << "/";
}

bool
CAwkNegatePattern::
exec()
{
  return ! pattern_->exec();
}

void
CAwkNegatePattern::
print(std::ostream &os) const
{
  os << "!" << *pattern_;
}

bool
CAwkExpressionPattern::
exec()
{
  auto value = expression_->getValue();

  return value->getBool();
}

void
CAwkExpressionPattern::
print(std::ostream &os) const
{
  os << *expression_;
}

bool
CAwkBeginPattern::
exec()
{
  return true;
}

bool
CAwkEndPattern::
exec()
{
  return true;
}

bool
CAwkCompositeOrPattern::
exec()
{
  return (pattern1_->exec() || pattern2_->exec());
}

void
CAwkCompositeOrPattern::
print(std::ostream &os) const
{
  os << *pattern1_ << " || " << *pattern2_;
}

bool
CAwkCompositeAndPattern::
exec()
{
  return (pattern1_->exec() && pattern2_->exec());
}

void
CAwkCompositeAndPattern::
print(std::ostream &os) const
{
  os << *pattern1_ << " && " << *pattern2_;
}

bool
CAwkRangePattern::
exec()
{
  if      (state_ == START_STATE) {
    if (pattern1_->exec()) {
      state_ = END_STATE;
      return true;
    }

    return false;
  }
  else if (state_ == END_STATE) {
    if (pattern2_->exec()) {
      state_ = DONE_STATE;
      return true;
    }

    return true;
  }
  else
    return false;
}

void
CAwkRangePattern::
print(std::ostream &os) const
{
  os << *pattern1_ << " , " << *pattern2_;
}
