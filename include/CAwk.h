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

#include <CStrParse.h>
#include <CFile.h>
#include <memory>

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
  enum class BlockFlags {
    NONE     = 0,
    BREAK    = (1<<0),
    CONTINUE = (1<<1),
    NEXT     = (1<<2),
    RETURN   = (1<<3),
    EXIT     = (1<<4)
  };

  typedef std::vector<CAwkFunctionPtr>      FunctionList;
  typedef std::vector<CAwkPatternActionPtr> PatternActionList;

  using ParseP = std::unique_ptr<CStrParse>;
  using FileP  = std::unique_ptr<CFile>;

  ParseP                  parser_;
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
  FileP                   input_file_;
  std::string             file_name_;
  int                     line_num_ { 0 };
  bool                    debug_ { false };
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

  void setBreakFlag  () { block_flags_ = BlockFlags::BREAK; }
  void resetBreakFlag() { block_flags_ = BlockFlags::NONE  ; }
  bool isBreakFlag   () { return (block_flags_ == BlockFlags::BREAK); }

  void setContinueFlag  () { block_flags_ = BlockFlags::CONTINUE; }
  void resetContinueFlag() { block_flags_ = BlockFlags::NONE  ; }
  bool isContinueFlag   () { return (block_flags_ == BlockFlags::CONTINUE); }

  void setNextFlag  () { block_flags_ = BlockFlags::NEXT; }
  void resetNextFlag() { block_flags_ = BlockFlags::NONE  ; }
  bool isNextFlag   () { return (block_flags_ == BlockFlags::NEXT); }

  void setReturnFlag  () { block_flags_ = BlockFlags::RETURN; }
  void resetReturnFlag() { block_flags_ = BlockFlags::NONE  ; }
  bool isReturnFlag   () { return (block_flags_ == BlockFlags::RETURN); }

  void setExitFlag  () { block_flags_ = BlockFlags::EXIT; }
  void resetExitFlag() { block_flags_ = BlockFlags::NONE  ; }
  bool isExitFlag   () { return (block_flags_ == BlockFlags::EXIT); }

  void error(const std::string &str) const;
};

#endif
