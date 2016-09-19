#ifndef CAWK_FUNCTION_H
#define CAWK_FUNCTION_H

#include <CAwkTypes.h>

class CAwkFunction {
 protected:
  std::string name_;

 protected:
  friend class CRefPtr<CAwkFunction>;

  CAwkFunction(const std::string &name) :
   name_(name) {
  }

  virtual ~CAwkFunction() { }

  CAwkFunction *dup() const { return NULL; }

 public:
  const std::string &getName() const { return name_; }

  virtual CAwkValuePtr exec(const CAwkExpressionTermList &values) = 0;

  virtual void print(std::ostream &os) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const CAwkFunction &th) {
    th.print(os); return os;
  }
};

class CAwkExprFunction : public CAwkExpressionTerm {
 private:
  std::string        name_;
  CAwkExpressionList expressionList_;

 public:
  static CAwkExpressionTermPtr
  create(const std::string &name, const CAwkExpressionList &expressionList) {
    return CAwkExpressionTermPtr(new CAwkExprFunction(name, expressionList));
  }

 private:
  friend class CRefPtr<CAwkExprFunction>;

  CAwkExprFunction(const std::string &name, const CAwkExpressionList &expressionList) :
   name_(name), expressionList_(expressionList) {
  }

 ~CAwkExprFunction() { }

  CAwkExprFunction *dup() const { return new CAwkExprFunction(*this); }

 public:
  bool hasValue() const { return true; }

  CAwkValuePtr getValue() const;

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const;
};

class CAwkParseFunction : public CAwkFunction {
 private:
  StringVectorT     args_;
  CAwkActionListPtr actionList_;

 public:
  static CAwkFunctionPtr
  create(const std::string &name, const StringVectorT &args, CAwkActionListPtr actionList) {
    return CAwkFunctionPtr(new CAwkParseFunction(name, args, actionList));
  }

 private:
  CAwkParseFunction(const std::string &name, const StringVectorT &args,
                    CAwkActionListPtr actionList) :
   CAwkFunction(name), args_(args), actionList_(actionList) {
  }

 ~CAwkParseFunction() { }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkBuiltinFunction : public CAwkFunction {
 protected:
  CAwkBuiltinFunction(const std::string &name) :
   CAwkFunction(name) {
  }

 public:
  virtual CAwkValuePtr exec(const CAwkExpressionTermList &values) = 0;

  virtual void print(std::ostream &os) const = 0;
};

class CAwkAtan2Function : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkAtan2Function);
  }

 private:
  CAwkAtan2Function() :
   CAwkBuiltinFunction("atan2") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkCosFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkCosFunction);
  }

 private:
  CAwkCosFunction() :
   CAwkBuiltinFunction("cos") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkExpFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkExpFunction);
  }

 private:
  CAwkExpFunction() :
   CAwkBuiltinFunction("exp") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkIntFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkIntFunction);
  }

 private:
  CAwkIntFunction() :
   CAwkBuiltinFunction("int") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkLogFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkLogFunction);
  }

 private:
  CAwkLogFunction() :
   CAwkBuiltinFunction("log") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkRandFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkRandFunction);
  }

 private:
  CAwkRandFunction() :
   CAwkBuiltinFunction("rand") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkSinFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkSinFunction);
  }

 private:
  CAwkSinFunction() :
   CAwkBuiltinFunction("sin") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkSqrtFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkSqrtFunction);
  }

 private:
  CAwkSqrtFunction() :
   CAwkBuiltinFunction("sqrt") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkSrandFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkSrandFunction);
  }

 private:
  CAwkSrandFunction() :
   CAwkBuiltinFunction("srand") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkGsubFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkGsubFunction());
  }

 private:
  CAwkGsubFunction() :
   CAwkBuiltinFunction("gsub") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkIndexFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkIndexFunction);
  }

 private:
  CAwkIndexFunction() :
   CAwkBuiltinFunction("index") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkLengthFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkLengthFunction);
  }

 private:
  CAwkLengthFunction() :
   CAwkBuiltinFunction("length") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkMatchFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkMatchFunction);
  }

 private:
  CAwkMatchFunction() :
   CAwkBuiltinFunction("match") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkSplitFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkSplitFunction);
  }

 private:
  CAwkSplitFunction() :
   CAwkBuiltinFunction("split") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkSprintfFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkSprintfFunction);
  }

 private:
  CAwkSprintfFunction() :
   CAwkBuiltinFunction("sprintf") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkSubFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkSubFunction);
  }

 private:
  CAwkSubFunction() :
   CAwkBuiltinFunction("sub") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkSubstrFunction : public CAwkBuiltinFunction {
 public:
  static CAwkFunctionPtr create() {
    return CAwkFunctionPtr(new CAwkSubstrFunction);
  }

 private:
  CAwkSubstrFunction() :
   CAwkBuiltinFunction("substr") {
  }

 public:
  CAwkValuePtr exec(const CAwkExpressionTermList &values);

  void print(std::ostream &os) const;
};

class CAwkFunctionMgr {
 private:
  typedef std::map<std::string,CAwkFunctionPtr> FunctionMap;

  FunctionMap functionMap_;

 public:
  CAwkFunctionMgr() { }

  void clear();

  void addFunction(CAwkFunctionPtr function);

  CAwkFunctionPtr getFunction(const std::string &name) const;

  void print(std::ostream &os) const;
};

#endif
