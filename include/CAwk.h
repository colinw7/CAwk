#ifndef CAWK_H
#define CAWK_H

#include <CAwkTypes.h>
#include <CAwkExecuteStack.h>
#include <CAwkVariable.h>
#include <CAwkValue.h>
#include <CAwkFunction.h>
#include <CAwkAction.h>
#include <CAwkPattern.h>
#include <CAwkOperator.h>

#include <CAutoPtr.h>
#include <CStrParse.h>
#include <CFile.h>

class CAwkPatternAction {
 private:
  CAwkPatternPtr    pattern_;
  CAwkActionListPtr actionList_;

 public:
  static CAwkPatternActionPtr
  create(CAwkPatternPtr pattern, CAwkActionListPtr actionList) {
    return CAwkPatternActionPtr(new CAwkPatternAction(pattern, actionList));
  }

 private:
  friend class CRefPtr<CAwkPatternAction>;

  CAwkPatternAction(CAwkPatternPtr pattern, CAwkActionListPtr actionList) :
   pattern_(pattern), actionList_(actionList) {
  }

 ~CAwkPatternAction() { }

  CAwkPatternAction *dup() const { return new CAwkPatternAction(*this); }

 public:
  bool isBegin() const;
  bool isEnd  () const;

  void exec();

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CAwkPatternAction &th) {
    th.print(os); return os;
  }
};

//--------------

#define CAwkInst CAwk::getInstance()

class CAwk {
 private:
  enum BlockFlags {
    NO_FLAG       = 0,
    BREAK_FLAG    = (1<<0),
    CONTINUE_FLAG = (1<<1),
    NEXT_FLAG     = (1<<2),
    RETURN_FLAG   = (1<<3),
    EXIT_FLAG     = (1<<4)
  };

  typedef std::vector<CAwkFunctionPtr>      FunctionList;
  typedef std::vector<CAwkPatternActionPtr> PatternActionList;

  CAutoPtr<CStrParse>     parser_;
  CAwkFunctionMgr         functionMgr_;
  CAwkVariableMgr         variableMgr_;
  CAwkFileMgr             fileMgr_;
  CAwkPipeMgr             pipeMgr_;
  PatternActionList       patternActionList_;
  CAwkExecuteStack        executeStack_;
  std::string             line_;
  COptValT<StringVectorT> lineFields_;
  std::string             output_field_separator_;
  std::string             output_record_separator_;
  std::string             real_output_format_;
  CAutoPtr<CFile>         input_file_;
  std::string             file_name_;
  int                     line_num_;
  bool                    debug_;
  CAwkActionBlockPtr      currentBlock_;
  CAwkActionBlockList     blockStack_;
  CAwkValuePtr            returnValue_;
  BlockFlags              block_flags_;

 public:
  static CAwk *getInstance();

 private:
  CAwk();
 ~CAwk() { }

 public:
  void init(const StringVectorT &args);

  bool parseFile(const std::string &fileName);
  bool parseLine(const std::string &line);

  bool parseInit(const std::string &str);

  bool execFile(const std::string &fileName);

  void addStdFunctions();
  void addStdVariables();

  bool parseProgram();

  void print(std::ostream &os) const;

  bool process();

  void setLine(const std::string &line);

  const std::string &getLine() { return line_; }

  const std::string &getLineField(uint pos) const;
  void setLineField(uint pos, const std::string &value);

  void setLineFields();

  CAwkExecuteStack &getExecuteStack() { return executeStack_; }

  bool getDebug() const { return debug_; }
  void setDebug(bool debug=true) { debug_ = debug; }

  CAwkValuePtr getReturnValue() const;
  void setReturnValue(CAwkValuePtr returnValue);

  friend std::ostream &operator<<(std::ostream &os, const CAwk &th) {
    th.print(os); return os;
  }

  bool parseFunction(CAwkFunctionPtr *function);
  bool parseFuncName(std::string &name);
  bool parseVarNameList(StringVectorT &args);
  bool parsePattern(CAwkPatternPtr *pattern);
  bool parseStatementList(CAwkActionListPtr *actionList);
  bool parseStatement(CAwkActionPtr *action);
  bool parseSimpleStatement(CAwkActionPtr *action);
  bool parseInputOutputAction(CAwkActionPtr *action);
  bool parseRedirectExpressionList(CAwkExpressionList &expressionList);
  bool parseExpressionList(CAwkExpressionList &expressionList);
  bool parseExpression(CAwkExpressionPtr *expression);
  bool parseExpressionValue(CAwkExpressionPtr *expression);
  bool parseExpressionTerm(CAwkExpressionTermPtr *term, bool isValue=false);
  bool parseRegularExpression(std::string &regexp);
  bool parseSubscript(CAwkExpressionPtr *expression);
  bool parseVariable(CAwkVariableRefPtr *var);
  bool parseIdentifier(std::string &name);
  bool parseFormat(std::string &name);
  bool parseString(CAwkValuePtr *value);
  bool parseString(std::string &str);
  bool parseNumber(CAwkValuePtr *value);
  bool parseOperator(CAwkOperatorPtr *op, bool value);

  int ioFindRedirect();

  bool isNewLine();
  bool readLine(std::string &line);

  std::string removeComments(const std::string &line);

  void addFunction(CAwkFunctionPtr function) {
    functionMgr_.addFunction(function);
  }

  CAwkFunctionPtr getFunction(const std::string &name) const {
    return functionMgr_.getFunction(name);
  }

  void addPatternAction(CAwkPatternActionPtr patternAction) {
    patternActionList_.push_back(patternAction);
  }

  CAwkValuePtr getValue(CAwkExpressionTermPtr term);
  CAwkVariableRefPtr getVariableRef(CAwkExpressionTermPtr term);

  CAwkVariablePtr getVariable(const std::string &name, bool create=false) const;
  CAwkVariablePtr addVariable(const std::string &name);

  CFile *getFile(FILE *file);
  CFile *getFile(const std::string &fileName, CFileBase::Mode mode);
  bool   closeFile(const std::string &fileName);

  CAwkPipe *getPipe(const std::string &cmdName, CAwkPipe::Type);
  bool      closePipe(const std::string &cmdName);

  void startBlock(CAwkActionBlockPtr block);
  void endBlock();

  void setBreakFlag  () { block_flags_ = BREAK_FLAG; }
  void resetBreakFlag() { block_flags_ = NO_FLAG  ; }
  bool isBreakFlag   () { return (block_flags_ & BREAK_FLAG); }

  void setContinueFlag  () { block_flags_ = CONTINUE_FLAG; }
  void resetContinueFlag() { block_flags_ = NO_FLAG  ; }
  bool isContinueFlag   () { return (block_flags_ & CONTINUE_FLAG); }

  void setNextFlag  () { block_flags_ = NEXT_FLAG; }
  void resetNextFlag() { block_flags_ = NO_FLAG  ; }
  bool isNextFlag   () { return (block_flags_ & NEXT_FLAG); }

  void setReturnFlag  () { block_flags_ = RETURN_FLAG; }
  void resetReturnFlag() { block_flags_ = NO_FLAG  ; }
  bool isReturnFlag   () { return (block_flags_ & RETURN_FLAG); }

  void setExitFlag  () { block_flags_ = EXIT_FLAG; }
  void resetExitFlag() { block_flags_ = NO_FLAG  ; }
  bool isExitFlag   () { return (block_flags_ & EXIT_FLAG); }

  void error(const std::string &str) const;
};

#endif
