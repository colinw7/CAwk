#ifndef CAWK_VALUE_H
#define CAWK_VALUE_H

#include <CAwkTypes.h>

class CAwkValue : public CAwkExpressionTerm {
 public:
  static CAwkValuePtr create(const std::string &value);
  static CAwkValuePtr create(const char *value);
  static CAwkValuePtr create(double value);
  static CAwkValuePtr create(int value);
  static CAwkValuePtr create(bool value);

 protected:
  explicit CAwkValue(const std::string &value);
  explicit CAwkValue(const char *value);
  explicit CAwkValue(double value);
  explicit CAwkValue(int value);
  explicit CAwkValue(bool value);

 public:
  virtual ~CAwkValue() { }

  CAwkValue *dup() const { return new CAwkValue(*this); }

  bool hasValue() const override { return true; }

  CAwkValuePtr getValue() const override { assert(false); }

  bool isReal   () const;
  bool isInteger() const;
  bool isBool   () const;

  std::string getString () const;
  double      getReal   () const;
  int         getInteger() const;
  bool        getBool   () const;

  void setValue  (CAwkValuePtr value);
  void setString (const std::string &value);
  void setReal   (double value);
  void setInteger(int value);
  void setBool   (bool value);

  int cmp(CAwkValuePtr rhs) const;

  CAwkExpressionTermPtr execute() override { assert(false); }

  void print(std::ostream &os) const override;

  friend std::ostream &operator<<(std::ostream &os, const CAwkValuePtr th) {
    th->print(os); return os;
  }

 protected:
  const std::string &getTrueStr() const {
    static std::string true_str = "1";

    return true_str;
  }

  const std::string &getFalseStr() const {
    static std::string false_str = "0";

    return false_str;
  }

 protected:
  std::string value_;
};

//----

class CAwkNullValue : public CAwkValue {
 public:
  static CAwkValuePtr create() {
    return CAwkValuePtr(new CAwkNullValue);
  }

 private:
  CAwkNullValue() :
   CAwkValue("") {
  }

 public:
  void print(std::ostream &os) const override {
    os << "<null>";
  }
};

#endif
