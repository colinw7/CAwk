#ifndef CAWK_ACTION_H
#define CAWK_ACTION_H

#include <CFile.h>

/*
 * Manager for opened files include special handling for stdin. stdout and stderr
 */
class CAwkFileMgr {
 public:
  CAwkFileMgr();
 ~CAwkFileMgr() { }

  void init();
  void term();

  CFile *getFile(FILE *file);
  CFile *getFile(const std::string &fileName, CFileBase::Mode mode);

  bool closeFile(const std::string &fileName);

 private:
  using FileList = std::list<CFile *>;

  FileList  files_;
  CFile*    stdInFile_  { nullptr };
  CFile*    stdOutFile_ { nullptr };
  CFile*    stdErrFile_ { nullptr };
};

//----

class CAwkPipe {
 public:
  enum class Type {
    NONE,
    INPUT,
    OUTPUT
  };

 public:
  CAwkPipe(const std::string &cmdName, Type type);
 ~CAwkPipe();

  const std::string &getCmdName() const { return cmdName_; }

  Type getType() const { return type_; }

  void addInput(const std::string &input) {
    input_ += input;
  }

  void setOutput(std::string *output) {
    output_ = output;
  }

  void close();

 private:
  std::string  cmdName_;
  Type         type_   { Type::NONE };
  bool         opened_ { false };
  std::string  input_;
  std::string *output_ { nullptr };
};

//----

class CAwkPipeMgr {
 public:
  CAwkPipeMgr() { }
 ~CAwkPipeMgr();

  void init();
  void term();

  CAwkPipe *getPipe(const std::string &cmdName, CAwkPipe::Type type);

  bool closePipe(const std::string &cmdName);

 private:
  using PipeList = std::list<CAwkPipe *>;

  PipeList pipes_;
};

//----

class CAwkIFile {
 public:
  enum class Type {
    READ_FILE    = 1,
    PIPE_COMMAND = 2
  };

 private:
  CAwkExpressionPtr file_;
  Type              type_;

 public:
  static CAwkIFilePtr create(CAwkExpressionPtr file, Type type) {
    return CAwkIFilePtr(new CAwkIFile(file, type));
  }

 private:
  CAwkIFile(CAwkExpressionPtr file, Type type) :
   file_(file), type_(type) {
  }

  CAwkIFile *dup() const { return nullptr; }

 public:
  bool read(std::string &str) const;

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkIFile &th) {
    th.print(os); return os;
  }

 private:
  CFile *getFile() const;
};

class CAwkOFile {
 public:
  enum class Type {
    WRITE_FILE   = 1,
    APPEND_FILE  = 2,
    PIPE_COMMAND = 3
  };

 private:
  CAwkExpressionPtr expression_;
  Type              type_;

 public:
  static CAwkOFilePtr create(CAwkExpressionPtr expression, Type type) {
    return CAwkOFilePtr(new CAwkOFile(expression, type));
  }

 private:
  CAwkOFile(CAwkExpressionPtr expression, Type type) :
   expression_(expression), type_(type) {
  }

  CAwkOFile *dup() const { return nullptr; }

 public:
  void write(const std::string &str);

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkOFile &th) {
    th.print(os); return os;
  }

 private:
  CFile *getFile() const;
};

//------------

class CAwkAction {
 protected:
  CAwkAction() { }

  CAwkAction *dup() const { return nullptr; }

 public:
  virtual ~CAwkAction() { }

  virtual void exec() = 0;

  virtual void print(std::ostream &os) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const CAwkAction &th) {
    th.print(os); return os;
  }
};

class CAwkNullAction : public CAwkAction {
 public:
  static CAwkActionPtr create() {
    return CAwkActionPtr(new CAwkNullAction);
  }

 private:
  CAwkNullAction() { }

 public:
  void exec() override;

  void print(std::ostream &) const override { }
};

class CAwkBreakAction : public CAwkAction {
 public:
  static CAwkActionPtr create() {
    return CAwkActionPtr(new CAwkBreakAction);
  }

 private:
  CAwkBreakAction() { }

 public:
  void exec() override;

  void print(std::ostream &os) const override { os << "break" << std::endl; }
};

class CAwkContinueAction : public CAwkAction {
 public:
  static CAwkActionPtr create() {
    return CAwkActionPtr(new CAwkContinueAction);
  }

 private:
  CAwkContinueAction() { }

 public:
  void exec() override;

  void print(std::ostream &os) const override { os << "continue" << std::endl; }
};

class CAwkNextAction : public CAwkAction {
 public:
  static CAwkActionPtr create() {
    return CAwkActionPtr(new CAwkNextAction);
  }

 private:
  CAwkNextAction() { }

 public:
  void exec() override;

  void print(std::ostream &os) const override { os << "next" << std::endl; }
};

class CAwkReturnAction : public CAwkAction {
 private:
  CAwkExpressionPtr expression_;

 public:
  static CAwkActionPtr create(CAwkExpressionPtr expression) {
    return CAwkActionPtr(new CAwkReturnAction(expression));
  }

 private:
  CAwkReturnAction(CAwkExpressionPtr expression) :
   expression_(expression) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkExitAction : public CAwkAction {
 private:
  CAwkExpressionPtr expression_;

 public:
  static CAwkActionPtr create(CAwkExpressionPtr expression) {
    return CAwkActionPtr(new CAwkExitAction(expression));
  }

 private:
  CAwkExitAction(CAwkExpressionPtr expression) :
   expression_(expression) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkDeleteAction : public CAwkAction {
 private:
  CAwkVariableRefPtr var_;
  CAwkExpressionPtr  expression_;

 public:
  static CAwkActionPtr
  create(CAwkVariableRefPtr var, CAwkExpressionPtr expression) {
    return CAwkActionPtr(new CAwkDeleteAction(var, expression));
  }

 private:
  CAwkDeleteAction(CAwkVariableRefPtr var, CAwkExpressionPtr expression) :
   var_(var), expression_(expression) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkIfAction : public CAwkAction {
 private:
  CAwkExpressionPtr expression_;
  CAwkActionListPtr actionList_;

 public:
  static CAwkActionPtr
  create(CAwkExpressionPtr expression, CAwkActionPtr action) {
    return CAwkActionPtr(new CAwkIfAction(expression, action));
  }

 private:
  CAwkIfAction(CAwkExpressionPtr expression, CAwkActionPtr action);

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkIfElseAction : public CAwkAction {
 private:
  CAwkExpressionPtr expression_;
  CAwkActionListPtr actionList1_;
  CAwkActionListPtr actionList2_;

 public:
  static CAwkActionPtr create(CAwkExpressionPtr expression,
                              CAwkActionPtr action1, CAwkActionPtr action2) {
    return CAwkActionPtr(new CAwkIfElseAction(expression, action1, action2));
  }

 private:
  CAwkIfElseAction(CAwkExpressionPtr expression,
                   CAwkActionPtr action1, CAwkActionPtr action2);

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkForAction : public CAwkAction {
 private:
  CAwkExpressionPtr expression1_;
  CAwkExpressionPtr expression2_;
  CAwkExpressionPtr expression3_;
  CAwkActionListPtr actionList_;

 public:
  static CAwkActionPtr
  create(CAwkExpressionPtr expression1, CAwkExpressionPtr expression2,
         CAwkExpressionPtr expression3, CAwkActionPtr action) {
    return CAwkActionPtr(new CAwkForAction(expression1, expression2,
                                           expression3, action));
  }

 private:
  CAwkForAction(CAwkExpressionPtr expression1, CAwkExpressionPtr expression2,
                CAwkExpressionPtr expression3, CAwkActionPtr action);

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkForInAction : public CAwkAction {
 private:
  CAwkVariableRefPtr var1_;
  CAwkVariableRefPtr var2_;
  CAwkActionListPtr  actionList_;

 public:
  static CAwkActionPtr create(CAwkVariableRefPtr var1, CAwkVariableRefPtr var2,
                              CAwkActionPtr action) {
    return CAwkActionPtr(new CAwkForInAction(var1, var2, action));
  }

 private:
  CAwkForInAction(CAwkVariableRefPtr var1, CAwkVariableRefPtr var2,
                  CAwkActionPtr action);

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkWhileAction : public CAwkAction {
 private:
  CAwkExpressionPtr expression_;
  CAwkActionListPtr actionList_;

 public:
  static CAwkActionPtr
  create(CAwkExpressionPtr expression, CAwkActionPtr action) {
    return CAwkActionPtr(new CAwkWhileAction(expression, action));
  }

 private:
  CAwkWhileAction(CAwkExpressionPtr expression, CAwkActionPtr action);

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkDoWhileAction : public CAwkAction {
 private:
  CAwkActionListPtr actionList_;
  CAwkExpressionPtr expression_;

 public:
  static CAwkActionPtr
  create(CAwkActionPtr action, CAwkExpressionPtr expression) {
    return CAwkActionPtr(new CAwkDoWhileAction(action, expression));
  }

 private:
  CAwkDoWhileAction(CAwkActionPtr action, CAwkExpressionPtr expression);

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkActionListAction : public CAwkAction {
 private:
  CAwkActionListPtr actionList_;

 public:
  static CAwkActionPtr create(CAwkActionListPtr actionList) {
    return CAwkActionPtr(new CAwkActionListAction(actionList));
  }

 private:
  CAwkActionListAction(CAwkActionListPtr actionList) :
   actionList_(actionList) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkExpressionAction : public CAwkAction {
 private:
  CAwkExpressionPtr expression_;

 public:
  static CAwkActionPtr create(CAwkExpressionPtr expression) {
    return CAwkActionPtr(new CAwkExpressionAction(expression));
  }

 private:
  CAwkExpressionAction(CAwkExpressionPtr expression) :
   expression_(expression) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkCloseAction : public CAwkAction {
 private:
  CAwkExpressionPtr expression_;

 public:
  static CAwkActionPtr create(CAwkExpressionPtr expression) {
    return CAwkActionPtr(new CAwkCloseAction(expression));
  }

 private:
  CAwkCloseAction(CAwkExpressionPtr expression) :
   expression_(expression) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkGetLineAction : public CAwkAction {
 private:
  CAwkVariableRefPtr var_;
  CAwkIFilePtr       file_;

 public:
  static CAwkActionPtr create(CAwkVariableRefPtr var, CAwkIFilePtr file) {
    return CAwkActionPtr(new CAwkGetLineAction(var, file));
  }

 private:
  CAwkGetLineAction(CAwkVariableRefPtr var, CAwkIFilePtr file) :
   var_(var), file_(file) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkPrintAction : public CAwkAction {
 private:
  CAwkExpressionList expressionList_;
  CAwkOFilePtr       file_;

 public:
  static CAwkActionPtr
  create(const CAwkExpressionList &expressionList, CAwkOFilePtr file) {
    return CAwkActionPtr(new CAwkPrintAction(expressionList, file));
  }

 private:
  CAwkPrintAction(const CAwkExpressionList &expressionList, CAwkOFilePtr file) :
   expressionList_(expressionList), file_(file) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkPrintFAction : public CAwkAction {
 private:
  CAwkExpressionList expressionList_;
  CAwkOFilePtr       file_;

 public:
  static CAwkActionPtr
  create(const CAwkExpressionList &expressionList, CAwkOFilePtr file) {
    return CAwkActionPtr(new CAwkPrintFAction(expressionList, file));
  }

 private:
  CAwkPrintFAction(const CAwkExpressionList &expressionList, CAwkOFilePtr file) :
   expressionList_(expressionList), file_(file) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkSystemAction : public CAwkAction {
 private:
  CAwkExpressionPtr expr_;

 public:
  static CAwkActionPtr create(CAwkExpressionPtr expr) {
    return CAwkActionPtr(new CAwkSystemAction(expr));
  }

 private:
  CAwkSystemAction(CAwkExpressionPtr expr) :
   expr_(expr) {
  }

 public:
  void exec() override;

  void print(std::ostream &os) const override;
};

class CAwkActionList {
 public:
  enum class Type {
    PROGRAM,
    ROUTINE,
    ITERATION,
    SIMPLE
  };

 public:
  static CAwkActionListPtr create(Type type) {
    return CAwkActionListPtr(new CAwkActionList(type));
  }

 private:
  CAwkActionList(Type type) : type_(type) { }

  CAwkActionList *dup() const { return new CAwkActionList(*this); }

 public:
  void addAction(CAwkActionPtr action);

  void exec();

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkActionList &th) {
    th.print(os); return os;
  }

 private:
  using ActionList = std::vector<CAwkActionPtr>;

  Type       type_;
  ActionList actionList_;
};

//----

class CAwkActionBlock {
 public:
  static CAwkActionBlockPtr
  create(CAwkActionListPtr actionList) {
    return CAwkActionBlockPtr(new CAwkActionBlock(actionList));
  }

 private:
  CAwkActionBlock(CAwkActionListPtr actionList);

  CAwkActionBlock *dup() const { return new CAwkActionBlock(*this); }

 public:
  CAwkVariablePtr getVariable(const std::string &name) const;
  CAwkVariablePtr addVariable(const std::string &name);

  void exec();

  CAwkValuePtr getReturnValue() const { return returnValue_; }
  void setReturnValue(CAwkValuePtr returnValue) { returnValue_ = returnValue; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkActionBlock &th) {
    th.print(os); return os;
  }

 private:
  CAwkActionListPtr actionList_;
  CAwkVariableMgr   variableMgr_;
  CAwkValuePtr      returnValue_;
};

#endif
