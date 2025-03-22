#ifndef CAWK_FUNCTION_H
#define CAWK_FUNCTION_H

#include <CAwkTypes.h>

class CAwkFunction {
 protected:
  CAwkFunction(CAwk *awk, const std::string &name) :
   awk_(awk), name_(name) {
  }

  CAwkFunction *dup() const { return NULL; }

 public:
  virtual ~CAwkFunction() { }

  const std::string &getName() const { return name_; }

  virtual CAwkValuePtr exec(const CAwkExpressionTermList &values) = 0;

  virtual void print(std::ostream &os) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const CAwkFunction &th) {
    th.print(os); return os;
  }

 protected:
  CAwk*       awk_ { nullptr };
  std::string name_;
};

//---

class CAwkExprFunction : public CAwkExpressionTerm {
 public:
  static CAwkExpressionTermPtr create(CAwk *awk, const std::string &name,
                                      const CAwkExpressionList &expressionList) {
    return CAwkExpressionTermPtr(new CAwkExprFunction(awk, name, expressionList));
  }

 private:
  CAwkExprFunction(CAwk *awk, const std::string &name, const CAwkExpressionList &expressionList) :
   awk_(awk), name_(name), expressionList_(expressionList) {
  }

  CAwkExprFunction *dup() const { return new CAwkExprFunction(*this); }

 public:
 ~CAwkExprFunction() { }

  bool hasValue() const override { return true; }

  CAwkValuePtr getValue() const override;

  CAwkExpressionTermPtr execute() override;

  void print(std::ostream &os) const override;

 private:
  CAwk*              awk_ { nullptr };
  std::string        name_;
  CAwkExpressionList expressionList_;
};

//----

class CAwkParseFunction : public CAwkFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk, const std::string &name, const StringVectorT &args,
                                CAwkActionListPtr actionList) {
    return CAwkFunctionPtr(new CAwkParseFunction(awk, name, args, actionList));
  }

 private:
  CAwkParseFunction(CAwk *awk, const std::string &name, const StringVectorT &args,
                    CAwkActionListPtr actionList) :
   CAwkFunction(awk, name), args_(args), actionList_(actionList) {
  }

 public:
 ~CAwkParseFunction() { }

  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;

 private:
  StringVectorT     args_;
  CAwkActionListPtr actionList_;
};

//----

class CAwkBuiltinFunction : public CAwkFunction {
 protected:
  CAwkBuiltinFunction(CAwk *awk, const std::string &name) :
   CAwkFunction(awk, name) {
  }
};

//----

class CAwkAtan2Function : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkAtan2Function(awk));
  }

 private:
  CAwkAtan2Function(CAwk *awk) :
   CAwkBuiltinFunction(awk, "atan2") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkCosFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkCosFunction(awk));
  }

 private:
  CAwkCosFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "cos") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkExpFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkExpFunction(awk));
  }

 private:
  CAwkExpFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "exp") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkIntFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkIntFunction(awk));
  }

 private:
  CAwkIntFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "int") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkLogFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkLogFunction(awk));
  }

 private:
  CAwkLogFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "log") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkRandFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkRandFunction(awk));
  }

 private:
  CAwkRandFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "rand") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkSinFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkSinFunction(awk));
  }

 private:
  CAwkSinFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "sin") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkSqrtFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkSqrtFunction(awk));
  }

 private:
  CAwkSqrtFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "sqrt") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkSrandFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkSrandFunction(awk));
  }

 private:
  CAwkSrandFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "srand") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkGsubFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkGsubFunction(awk));
  }

 private:
  CAwkGsubFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "gsub") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkIndexFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkIndexFunction(awk));
  }

 private:
  CAwkIndexFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "index") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkLengthFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkLengthFunction(awk));
  }

 private:
  CAwkLengthFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "length") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkMatchFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkMatchFunction(awk));
  }

 private:
  CAwkMatchFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "match") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkSplitFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkSplitFunction(awk));
  }

 private:
  CAwkSplitFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "split") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkSprintfFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkSprintfFunction(awk));
  }

 private:
  CAwkSprintfFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "sprintf") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkSubFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkSubFunction(awk));
  }

 private:
  CAwkSubFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "sub") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkSubstrFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create(CAwk *awk) {
    return CAwkFunctionPtr(new CAwkSubstrFunction(awk));
  }

 private:
  CAwkSubstrFunction(CAwk *awk) :
   CAwkBuiltinFunction(awk, "substr") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values) override;

  void print(std::ostream &os) const override;
};

//----

class CAwkFunctionMgr {
 public:
  CAwkFunctionMgr() { }

  void clear();

  void addFunction(CAwkFunctionPtr function);

  CAwkFunctionPtr getFunction(const std::string &name) const;

  void print(std::ostream &os) const;

 private:
  using FunctionMap = std::map<std::string,CAwkFunctionPtr>;

  FunctionMap functionMap_;
};

#endif
