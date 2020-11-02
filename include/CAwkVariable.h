#ifndef CCAWK_VARIABLE_H
#define CCAWK_VARIABLE_H

#include <CAwkTypes.h>

class CAwkVariableMgr {
 public:
  CAwkVariableMgr() { }

  void addVariable(const std::string &name, const std::string &value);
  void addVariable(CAwkVariablePtr var);

  CAwkVariablePtr getVariable(const std::string &name) const;

  void print(std::ostream &os) const;

 private:
  using VariableMap = std::map<std::string,CAwkVariablePtr>;

  VariableMap variableMap_;
};

//----

class CAwkVariable {
 public:
  static CAwkVariablePtr create(const std::string &name, const std::string &value);
  static CAwkVariablePtr create(const std::string &name, const char *value);
  static CAwkVariablePtr create(const std::string &name, double value);
  static CAwkVariablePtr create(const std::string &name, int value);
  static CAwkVariablePtr create(const std::string &name, bool value);

 protected:
  friend class CRefPtr<CAwkVariable>;

  explicit CAwkVariable(const std::string &name, const std::string &value);
  explicit CAwkVariable(const std::string &name, const char *value);
  explicit CAwkVariable(const std::string &name, double value);
  explicit CAwkVariable(const std::string &name, int value);
  explicit CAwkVariable(const std::string &name, bool value);

  virtual ~CAwkVariable() { }

  CAwkVariable *dup() const { return new CAwkVariable(*this); }

 public:
  const std::string &getName() const { return name_; }

  CAwkValuePtr getValue() const;
  virtual void setValue(CAwkValuePtr value);

  CAwkValuePtr getIndValue(const std::string &ind) const;
  virtual void setIndValue(const std::string &ind, CAwkValuePtr value);

  bool isInd(const std::string &ind) const;

  void removeInd(const std::string &ind);

  StringVectorT getIndices() const;

  CAwkExpressionTermPtr execute();

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkVariable &th) {
    th.print(os); return os;
  }

 private:
  using IndValueMap = std::map<std::string,CAwkValuePtr>;

  std::string  name_;
  CAwkValuePtr value_;
  IndValueMap  indValueMap_;
};

//----

class CAwkARGCVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("ARGC", 0);
  }
};

//----

class CAwkARGVVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("ARGV", "");
  }
};

//----

class CAwkFILENAMEVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("FILENAME", "");
  }
};

//----

class CAwkFNRVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("FNR", 0);
  }
};

//----

class CAwkFSVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("FS", " ");
  }
};

//----

class CAwkNFVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("NF", 0);
  }
};

//----

class CAwkNRVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("NR", 0);
  }
};

//----

class CAwkOFMTVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("OFMT", "%.6g");
  }
};

//----

class CAwkOFSVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("OFS", " ");
  }
};

//----

class CAwkORSVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("ORS", "\n");
  }
};

//----

class CAwkRLENGTHVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("RLENGTH", 0);
  }
};

//----

class CAwkRSVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("RS", "\n");
  }
};

//----

class CAwkRSTARTVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("RSTART", 0);
  }
};

//----

class CAwkSUBSEPVariable {
 public:
  static CAwkVariablePtr create() {
    return CAwkVariable::create("SUBSEP", "\034");
  }
};

//----------

#include <CAwkExpression.h>

class CAwkVariableRef : public CAwkExpressionTerm {
 public:
  static CAwkVariableRefPtr create(const std::string &name) {
    return CAwkVariableRefPtr(new CAwkVariableRef(name));
  }

 protected:
  friend class CRefPtr<CAwkVariableRef>;

  CAwkVariableRef(const std::string &name) :
   name_(name) {
  }

  virtual ~CAwkVariableRef() { }

  CAwkVariableRef *dup() const { return new CAwkVariableRef(*this); }

 public:
  bool hasValue() const { return true; }

  void instantiate(bool global=false);

  virtual CAwkValuePtr getValue() const;

  virtual void setValue(CAwkValuePtr value);

  CAwkValuePtr getIndValue(const std::string &ind) const;
  void setIndValue(const std::string &ind, CAwkValuePtr value);

  bool isInd(const std::string &ind) const;

  void removeInd(const std::string &ind);

  StringVectorT getIndices() const;

  virtual void print(std::ostream &os) const;

  virtual CAwkExpressionTermPtr execute();

 private:
  std::string name_;
};

//---

class CAwkArrayVariableRef : public CAwkVariableRef {
 public:
  static CAwkVariableRefPtr create(const std::string &name,
                                   const CAwkExpressionList &expressionList) {
    return CAwkVariableRefPtr(new CAwkArrayVariableRef(name, expressionList));
  }

 private:
  CAwkArrayVariableRef(const std::string &name, const CAwkExpressionList &expressionList) :
   CAwkVariableRef(name), expressionList_(expressionList) {
  }

 public:
  CAwkValuePtr getValue() const;

  void setValue(CAwkValuePtr value);

  void print(std::ostream &os) const;

  CAwkExpressionTermPtr execute();

 private:
  std::string getInd() const;

 private:
  CAwkExpressionList expressionList_;
};

//---

class CAwkFieldVariableRef : public CAwkVariableRef {
 public:
  static CAwkVariableRefPtr create(int pos) {
    return CAwkVariableRefPtr(new CAwkFieldVariableRef(pos));
  }

 private:
  CAwkFieldVariableRef(int pos) :
   CAwkVariableRef(""), pos_(pos) {
  }

 public:
  CAwkValuePtr getValue() const;

  void setValue(CAwkValuePtr value);

  void print(std::ostream &os) const;

  CAwkExpressionTermPtr execute();

 private:
  int pos_ { 0 };
};

#endif
