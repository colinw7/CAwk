#ifndef CAWK_PATTERN_H
#define CAWK_PATTERN_H

#include <CRegExp.h>

class CAwkPattern {
 protected:
  CAwkPattern() { }

  CAwkPattern *dup() const { return NULL; }

 public:
  virtual ~CAwkPattern() { }

  virtual bool exec() = 0;

  virtual void print(std::ostream &os) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const CAwkPattern &th) {
    th.print(os); return os;
  }
};

//---

class CAwkNullPattern : public CAwkPattern {
 public:
  static CAwkPatternPtr create() {
    return CAwkPatternPtr(new CAwkNullPattern);
  }

 private:
  CAwkNullPattern() { }

 public:
  bool exec() override { return true; }

  void print(std::ostream &) const override { }
};

//---

class CAwkRegExpPattern : public CAwkPattern {
 public:
  static CAwkPatternPtr create(const std::string &regexp) {
    return CAwkPatternPtr(new CAwkRegExpPattern(regexp));
  }

 private:
  CAwkRegExpPattern(const std::string &regexp) :
   regexp_(regexp) {
    regexp_.setExtended(true);
  }

 public:
  bool exec() override;

  void print(std::ostream &os) const override;

 private:
  CRegExp regexp_;
};

//---

class CAwkNegatePattern : public CAwkPattern {
 public:
  static CAwkPatternPtr create(CAwkPatternPtr pattern) {
    return CAwkPatternPtr(new CAwkNegatePattern(pattern));
  }

 private:
  CAwkNegatePattern(CAwkPatternPtr pattern) :
   pattern_(pattern) {
  }

 public:
  bool exec() override;

  void print(std::ostream &os) const override;

 private:
  CAwkPatternPtr pattern_;
};

//---

class CAwkBeginPattern : public CAwkPattern {
 public:
  static CAwkPatternPtr create() {
    return CAwkPatternPtr(new CAwkBeginPattern);
  }

 private:
  CAwkBeginPattern() { }

 public:
  bool exec() override;

  void print(std::ostream &os) const override { os << "BEGIN"; }
};

//---

class CAwkEndPattern : public CAwkPattern {
 public:
  static CAwkPatternPtr create() {
    return CAwkPatternPtr(new CAwkEndPattern);
  }

 private:
  CAwkEndPattern() { }

 public:
  bool exec() override;

  void print(std::ostream &os) const override { os << "END"; }
};

//---

class CAwkExpressionPattern : public CAwkPattern {
 public:
  static CAwkPatternPtr create(CAwkExpressionPtr expression) {
    return CAwkPatternPtr(new CAwkExpressionPattern(expression));
  }

 private:
  CAwkExpressionPattern(CAwkExpressionPtr expression) :
   expression_(expression) {
  }

 public:
  bool exec() override;

  void print(std::ostream &os) const override;

 private:
  CAwkExpressionPtr expression_;
};

//---

class CAwkCompositeOrPattern : public CAwkPattern {
 public:
  static CAwkPatternPtr
  create(CAwkPatternPtr pattern1, CAwkPatternPtr pattern2) {
    return CAwkPatternPtr(new CAwkCompositeOrPattern(pattern1, pattern2));
  }

 private:
  CAwkCompositeOrPattern(CAwkPatternPtr pattern1, CAwkPatternPtr pattern2) :
   pattern1_(pattern1), pattern2_(pattern2) {
  }

 public:
  bool exec() override;

  void print(std::ostream &os) const override;

 private:
  CAwkPatternPtr pattern1_;
  CAwkPatternPtr pattern2_;
};

//---

class CAwkCompositeAndPattern : public CAwkPattern {
 public:
  static CAwkPatternPtr
  create(CAwkPatternPtr pattern1, CAwkPatternPtr pattern2) {
    return CAwkPatternPtr(new CAwkCompositeAndPattern(pattern1, pattern2));
  }

 private:
  CAwkCompositeAndPattern(CAwkPatternPtr pattern1, CAwkPatternPtr pattern2) :
   pattern1_(pattern1), pattern2_(pattern2) {
  }

 public:
  bool exec() override;

  void print(std::ostream &os) const override;

 private:
  CAwkPatternPtr pattern1_;
  CAwkPatternPtr pattern2_;
};

//---

class CAwkRangePattern : public CAwkPattern {
 private:
  enum State {
    START_STATE,
    END_STATE,
    DONE_STATE
  };

 public:
  static CAwkPatternPtr
  create(CAwkPatternPtr pattern1, CAwkPatternPtr pattern2) {
    return CAwkPatternPtr(new CAwkRangePattern(pattern1, pattern2));
  }

 private:
  CAwkRangePattern(CAwkPatternPtr pattern1, CAwkPatternPtr pattern2) :
   pattern1_(pattern1), pattern2_(pattern2), state_(START_STATE) {
  }

 public:
  bool exec() override;

  void print(std::ostream &os) const override;

 public:
  CAwkPatternPtr pattern1_;
  CAwkPatternPtr pattern2_;
  State          state_ { START_STATE };
};

#endif
