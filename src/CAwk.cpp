#include <CAwk.h>
#include <CStrParse.h>
#include <CStrUtil.h>
#include <CFuncs.h>
#include <CFile.h>
#include <cstdio>
#include <cstring>

CAwk *
CAwk::
getInstance()
{
  static CAwk *instance;

  if (! instance)
    instance = new CAwk;

  return instance;
}

CAwk::
CAwk()
{
  parser_ = std::make_unique<CStrParse>();
}

void
CAwk::
init(const StringVectorT &args)
{
  addStdVariables();
  addStdFunctions();

  uint argc = args.size();

  getVariable("ARGC")->setValue(CAwkValue::create((int) argc));

  for (uint i = 0; i < argc; ++i)
    getVariable("ARGV")->setIndValue(CStrUtil::toString(i),
                                     CAwkValue::create(args[i]));
}

bool
CAwk::
parseFile(const std::string &fileName)
{
  if (! CFile::exists(fileName) || ! CFile::isRegular(fileName)) {
    error("Invalid file '" + fileName + "'");
    return false;
  }

  bool rc = true;

  file_name_ = fileName;
  line_num_  = 0;

  input_file_ = std::make_unique<CFile>(fileName);

  std::string line;

  while (readLine(line)) {
    line = CStrUtil::stripSpaces(line);

    if (line == "")
      continue;

    if (! (rc = parseLine(line)))
      break;

    if (getDebug())
      std::cout << *this << std::endl;
  }

  input_file_ = nullptr;

  return rc;
}

bool
CAwk::
parseLine(const std::string &str)
{
  if (getDebug())
    std::cout << str << std::endl;

  parseInit(str);

  if (! parseProgram())
    return false;

  if (getDebug())
    std::cout << *this << std::endl;

  return true;
}

bool
CAwk::
parseInit(const std::string &str)
{
  parser_->setString(str);

  return true;
}

void
CAwk::
addStdVariables()
{
  variableMgr_.addVariable(CAwkARGCVariable    ::create());
  variableMgr_.addVariable(CAwkARGVVariable    ::create());
  variableMgr_.addVariable(CAwkFILENAMEVariable::create());
  variableMgr_.addVariable(CAwkFNRVariable     ::create());
  variableMgr_.addVariable(CAwkFSVariable      ::create());
  variableMgr_.addVariable(CAwkNFVariable      ::create());
  variableMgr_.addVariable(CAwkNRVariable      ::create());
  variableMgr_.addVariable(CAwkOFMTVariable    ::create());
  variableMgr_.addVariable(CAwkOFSVariable     ::create());
  variableMgr_.addVariable(CAwkORSVariable     ::create());
  variableMgr_.addVariable(CAwkRLENGTHVariable ::create());
  variableMgr_.addVariable(CAwkRSVariable      ::create());
  variableMgr_.addVariable(CAwkRSTARTVariable  ::create());
  variableMgr_.addVariable(CAwkSUBSEPVariable  ::create());
}

void
CAwk::
addStdFunctions()
{
  functionMgr_.addFunction(CAwkGsubFunction   ::create());
  functionMgr_.addFunction(CAwkIndexFunction  ::create());
  functionMgr_.addFunction(CAwkLengthFunction ::create());
  functionMgr_.addFunction(CAwkMatchFunction  ::create());
  functionMgr_.addFunction(CAwkSplitFunction  ::create());
  functionMgr_.addFunction(CAwkSprintfFunction::create());
  functionMgr_.addFunction(CAwkSubFunction    ::create());
  functionMgr_.addFunction(CAwkSubstrFunction ::create());

  functionMgr_.addFunction(CAwkAtan2Function  ::create());
  functionMgr_.addFunction(CAwkCosFunction    ::create());
  functionMgr_.addFunction(CAwkExpFunction    ::create());
  functionMgr_.addFunction(CAwkIntFunction    ::create());
  functionMgr_.addFunction(CAwkLogFunction    ::create());
  functionMgr_.addFunction(CAwkRandFunction   ::create());
  functionMgr_.addFunction(CAwkSinFunction    ::create());
  functionMgr_.addFunction(CAwkSqrtFunction   ::create());
  functionMgr_.addFunction(CAwkSrandFunction  ::create());
}

bool
CAwk::
parseProgram()
{
  // function <funcname> ( <varname_list> ) { <statement_list> }
  if (parser_->isWord("function")) {
    CAwkFunctionPtr function;

    if (! parseFunction(&function))
      return false;

    addFunction(function);
  }
  else {
    CAwkPatternPtr pattern;

    if (! parsePattern(&pattern))
      return false;

    parser_->skipSpace();

    CAwkActionListPtr actionList =
      CAwkActionList::create(CAwkActionList::Type::PROGRAM);

    if (parser_->isChar('{')) {
      if (! parseStatementList(&actionList))
        return false;
    }
    else {
      CAwkActionPtr action = CAwkNullAction::create();

      actionList->addAction(action);
    }

    CAwkPatternActionPtr patternAction =
      CAwkPatternAction::create(pattern, actionList);

    addPatternAction(patternAction);
  }

  parser_->skipSpace();

  return parser_->eof();
}

void
CAwk::
print(std::ostream &os) const
{
  for_each(patternActionList_.begin(), patternActionList_.end(),
           CPrintSeparated<CAwkPatternActionPtr>(os));
}

bool
CAwk::
process()
{
  fileMgr_.init();
  pipeMgr_.init();

  uint num_begin = 0;
  uint num_body  = 0;
  uint num_end   = 0;

  // begin
  {
    PatternActionList::iterator p1 = patternActionList_.begin();
    PatternActionList::iterator p2 = patternActionList_.end  ();

    for ( ; p1 != p2; ++p1) {
      if      ((*p1)->isBegin()) {
        (*p1)->exec();

        ++num_begin;
      }
      else if ((*p1)->isEnd())
        ++num_end;
      else
        ++num_body;
    }
  }

  if (num_body != 0 || num_end != 0) {
    // body
    StringVectorT indices = getVariable("ARGV")->getIndices();

    if (indices.size() > 1) {
      StringVectorT::const_iterator p1 = indices.begin();
      StringVectorT::const_iterator p2 = indices.end  ();

      ++p1;

      for ( ; p1 != p2; ++p1) {
        std::string fileName = getVariable("ARGV")->getIndValue(*p1)->getString();

        (void) execFile(fileName);
      }
    }
    else
      (void) execFile("-");
  }

  // end
  {
    PatternActionList::iterator p1 = patternActionList_.begin();
    PatternActionList::iterator p2 = patternActionList_.end  ();

    for ( ; p1 != p2; ++p1) {
      if (! (*p1)->isEnd())
        continue;

      (*p1)->exec();
    }
  }

  // cleanup
  fileMgr_.term();
  pipeMgr_.term();

  return true;
}

bool
CAwk::
execFile(const std::string &fileName)
{
  FileP file;

  if (fileName == "-") // stdin
    file = std::make_unique<CFile>(stdin);
  else {
    if (! CFile::exists(fileName) || ! CFile::isRegular(fileName)) {
      error("Invalid file '" + fileName + "'");
      return false;
    }

    file = std::make_unique<CFile>(fileName);
  }

  getVariable("FILENAME")->getValue()->setString(fileName);

  std::string line;

  while (file->readLine(line)) {
    setLine(line);

    PatternActionList::iterator p1 = patternActionList_.begin();
    PatternActionList::iterator p2 = patternActionList_.end  ();

    for ( ; p1 != p2; ++p1) {
      if ((*p1)->isBegin() || (*p1)->isEnd())
        continue;

      (*p1)->exec();

      if (isNextFlag()) {
        resetNextFlag();
        break;
      }
    }
  }

  return true;
}

void
CAwk::
setLine(const std::string &line)
{
  line_ = line;

  setLineFields();

  getVariable("FNR")->getValue()->setInteger(
    getVariable("FNR")->getValue()->getInteger() + 1);

  getVariable("NR")->getValue()->setInteger(
    getVariable("NR")->getValue()->getInteger() + 1);
}

const std::string &
CAwk::
getLineField(uint pos) const
{
  static std::string null_str;

  if (pos == 0)
    return line_;

  if (! lineFields_.isValid()) {
    CAwk *th = const_cast<CAwk *>(this);

    th->setLineFields();
  }

  if (pos <= lineFields_.getValue().size())
    return (lineFields_.getValue())[pos - 1];
  else
    return null_str;
}

void
CAwk::
setLineField(uint pos, const std::string &value)
{
  if (pos == 0) {
    line_ = value;

    setLineFields();

    return;
  }

  if (! lineFields_.isValid())
    setLineFields();

  StringVectorT &fields = lineFields_.getValue();

  if (pos > fields.size()) {
    while (pos > fields.size())
      fields.push_back("");

    getVariable("NF")->getValue()->setInteger(
      lineFields_.getValue().size());
  }

  fields[pos - 1] = value;

  line_ = "";

  StringVectorT::const_iterator p1 = fields.begin();
  StringVectorT::const_iterator p2 = fields.end  ();

  for (int i = 0; p1 != p2; ++p1, ++i) {
    if (i > 0)
      line_ += getVariable("OFS")->getValue()->getString();

    line_ += *p1;
  }
}

void
CAwk::
setLineFields()
{
  StringVectorT fields;

  std::string fs = getVariable("FS")->getValue()->getString();

  if (fs == " ")
    fs = " \t";

  CStrUtil::addFields(line_, fields, fs, /*skipEmpty*/true);

  lineFields_.setValue(fields);

  getVariable("NF")->getValue()->setInteger(
    lineFields_.getValue().size());
}

CAwkValuePtr
CAwk::
getReturnValue() const
{
  if (currentBlock_.isValid())
    return currentBlock_->getReturnValue();
  else
    return returnValue_;
}

void
CAwk::
setReturnValue(CAwkValuePtr returnValue)
{
  if (currentBlock_.isValid())
    currentBlock_->setReturnValue(returnValue);
  else
    returnValue_ = returnValue;
}

// function <funcname> ( <varname_list> ) { <statement_list> }
bool
CAwk::
parseFunction(CAwkFunctionPtr *function)
{
  // skip 'function'
  parser_->skipNonSpace();

  parser_->skipSpace();

  // get function name
  std::string name;

  if (! parseFuncName(name))
    return false;

  parser_->skipSpace();

  // read argument list
  if (! parser_->isChar('(')) {
    error("Expected '('");
    return false;
  }

  parser_->skipChar();

  StringVectorT args;

  if (! parseVarNameList(args))
    return false;

  parser_->skipSpace();

  if (! parser_->isChar(')')) {
    error("Expected ')'");
    return false;
  }

  parser_->skipChar();

  // read function body
  CAwkActionListPtr actionList =
    CAwkActionList::create(CAwkActionList::Type::ROUTINE);

  if (! parseStatementList(&actionList))
    return false;

  *function = CAwkParseFunction::create(name, args, actionList);

  return true;
}

// <funcname>
bool
CAwk::
parseFuncName(std::string &name)
{
  return parseIdentifier(name);
}

bool
CAwk::
parseVarNameList(StringVectorT &args)
{
  if (parser_->isChar(')'))
    return true;

  std::string arg;

  if (! parseIdentifier(arg))
    return false;

  args.push_back(arg);

  parser_->skipSpace();

  while (parser_->isChar(',')) {
    parser_->skipChar();

    arg = "";

    if (! parseIdentifier(arg))
      return false;

    args.push_back(arg);

    parser_->skipSpace();
  }

  return true;
}

bool
CAwk::
parsePattern(CAwkPatternPtr *pattern)
{
  parser_->skipSpace();

  if      (parser_->isChar('{')) {
    *pattern = CAwkNullPattern::create();
  }
  else if (parser_->isChar('/')) {
    std::string regexp;

    if (! parseRegularExpression(regexp))
      return false;

    *pattern = CAwkRegExpPattern::create(regexp);
  }
  else if (parser_->isChar('!')) {
    parser_->skipChar();

    CAwkPatternPtr pattern1;

    if (! parsePattern(&pattern1))
      return false;

    *pattern = CAwkNegatePattern::create(pattern1);
  }
  // ( <pattern> )
  else if (parser_->isChar('(')) {
    parser_->skipChar();

    CAwkPatternPtr pattern1;

    if (! parsePattern(&pattern1))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(')')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    *pattern = pattern1;
  }
  else if (parser_->isWord("BEGIN")) {
    parser_->skipChars(5);

    *pattern = CAwkBeginPattern::create();
  }
  else if (parser_->isWord("END")) {
    parser_->skipChars(3);

    *pattern = CAwkEndPattern::create();
  }
  else {
    CAwkExpressionPtr expression;

    if (! parseExpression(&expression))
      return false;

    *pattern = CAwkExpressionPattern::create(expression);
  }

  parser_->skipSpace();

  if      (parser_->isChar('|')) {
    CAwkPatternPtr pattern1 = *pattern;

    parser_->skipChar();

    if (! parser_->isChar('|')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    CAwkPatternPtr pattern2;

    if (! parsePattern(&pattern2))
      return false;

    *pattern = CAwkCompositeOrPattern::create(pattern1, pattern2);
  }
  else if (parser_->isChar('&')) {
    CAwkPatternPtr pattern1 = *pattern;

    parser_->skipChar();

    if (! parser_->isChar('&')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    CAwkPatternPtr pattern2;

    if (! parsePattern(&pattern2))
      return false;

    *pattern = CAwkCompositeAndPattern::create(pattern1, pattern2);
  }
  else if (parser_->isChar(',')) {
    CAwkPatternPtr pattern1 = *pattern;

    parser_->skipChar();

    CAwkPatternPtr pattern2;

    if (! parsePattern(&pattern2))
      return false;

    *pattern = CAwkRangePattern::create(pattern1, pattern2);
  }

  return true;
}

// <statement>
// <statement_list> <statement>
bool
CAwk::
parseStatementList(CAwkActionListPtr *actionList)
{
  CAwkActionPtr action;

  parser_->skipSpace();

  if (parser_->isChar('{')) {
    parser_->skipChar();

    if (isNewLine())
      parser_->skipChar();

    while (true) {
      if (! parseStatement(&action))
        return false;

      (*actionList)->addAction(action);

      parser_->skipSpace();

      if (parser_->isChar(';')) {
        parser_->skipChar();
        continue;
      }

      if (isNewLine()) {
        parser_->skipChar();

        parser_->skipSpace();

        if (! parser_->isChar('}'))
          continue;
      }

      break;
    }

    parser_->skipSpace();

    if (! parser_->isChar('}')) {
      error("Expected '}'");
      return false;
    }

    parser_->skipChar();
  }
  else {
    if (! parseStatement(&action))
      return false;

    (*actionList)->addAction(action);
  }

  return true;
}

bool
CAwk::
parseStatement(CAwkActionPtr *action)
{
  parser_->skipSpace();

  if      (parser_->isChar('}'))
    return false;

  //-------

  // break
  if      (parser_->isWord("break")) {
    parser_->skipChars(5);

    *action = CAwkBreakAction::create();
  }
  // continue
  else if (parser_->isWord("continue")) {
    parser_->skipChars(8);

    *action = CAwkContinueAction::create();
  }
  // do <statement> while ( <pattern> )
  else if (parser_->isWord("do")) {
    parser_->skipChars(2);

    CAwkActionPtr action1;

    if (! parseStatement(&action1))
      return false;

    parser_->skipSpace();

    if (! parser_->isWord("while")) {
      error("Expected 'while'");
      return false;
    }

    parser_->skipChars(5);

    parser_->skipSpace();

    if (! parser_->isChar('(')) {
      error("Expected '('");
      return false;
    }

    parser_->skipChar();

    CAwkExpressionPtr expression;

    if (! parseExpression(&expression))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(')')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    *action = CAwkDoWhileAction::create(action1, expression);
  }
  // exit <pattern>
  // exit
  else if (parser_->isWord("exit")) {
    parser_->skipChars(4);

    CAwkExpressionPtr expression;

    (void) parseExpression(&expression);

    *action = CAwkExitAction::create(expression);
  }
  // for
  else if (parser_->isWord("for")) {
    parser_->skipChars(3);

    parser_->skipSpace();

    if (! parser_->isChar('(')) {
      error("Expected '('");
      return false;
    }

    parser_->skipChar();

    // try var 'in' var_array
    int save_pos = parser_->getPos();

    CAwkVariableRefPtr var1;

    if (parseVariable(&var1)) {
      parser_->skipSpace();

      if (parser_->isWord("in")) {
        parser_->skipChars(2);

        CAwkVariableRefPtr var2;

        if (parseVariable(&var2)) {
          parser_->skipSpace();

          if (parser_->isChar(')')) {
            parser_->skipChar();

            if (isNewLine())
              parser_->skipChar();

            CAwkActionPtr action1;

            if (! parseStatement(&action1))
              return false;

            *action = CAwkForInAction::create(var1, var2, action1);

            return true;
          }
        }
      }
    }

    parser_->setPos(save_pos);

    //-----

    CAwkExpressionPtr expression1;

    if (! parseExpression(&expression1))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(';')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    CAwkExpressionPtr expression2;

    if (! parseExpression(&expression2))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(';')) {
      error("Expected ';'");
      return false;
    }

    parser_->skipChar();

    CAwkExpressionPtr expression3;

    if (! parseExpression(&expression3))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(')')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    if (isNewLine())
      parser_->skipChar();

    CAwkActionPtr action1;

    if (! parseStatement(&action1))
      return false;

    *action = CAwkForAction::create(expression1, expression2,
                                    expression3, action1);
  }
  // if <statement> else <statement>
  // if <statement>
  else if (parser_->isWord("if")) {
    parser_->skipChars(2);

    parser_->skipSpace();

    if (! parser_->isChar('(')) {
      error("Expected '('");
      return false;
    }

    parser_->skipChar();

    CAwkExpressionPtr expression;

    if (! parseExpression(&expression))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(')')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    if (isNewLine())
      parser_->skipChar();

    CAwkActionPtr action1;

    if (! parseStatement(&action1))
      return false;

    int save_pos = parser_->getPos();

    if (isNewLine())
      parser_->skipChar();

    if (parser_->isWord("else")) {
      parser_->skipChars(4);

      if (isNewLine())
        parser_->skipChar();

      CAwkActionPtr action2;

      if (! parseStatement(&action2))
        return false;

      *action = CAwkIfElseAction::create(expression, action1, action2);
    }
    else {
      parser_->setPos(save_pos);

      *action = CAwkIfAction::create(expression, action1);
    }
  }
  // { <statement_list> }
  else if (parser_->isChar('{')) {
    CAwkActionListPtr actionList =
      CAwkActionList::create(CAwkActionList::Type::SIMPLE);

    if (! parseStatementList(&actionList))
      return false;

    *action = CAwkActionListAction::create(actionList);
  }
  // next
  else if (parser_->isWord("next")) {
    parser_->skipChars(4);

    *action = CAwkNextAction::create();
  }
  // nextfile ??
  // return <pattern>
  // return
  else if (parser_->isWord("return")) {
    parser_->skipChars(6);

    CAwkExpressionPtr expression;

    (void) parseExpression(&expression);

    *action = CAwkReturnAction::create(expression);
  }
  // <simple_statement>
  else if (parseSimpleStatement(action)) {
  }
  // while <statement>
  else if (parser_->isWord("while")) {
    parser_->skipChars(5);

    parser_->skipSpace();

    if (! parser_->isChar('(')) {
      error("Expected '('");
      return false;
    }

    parser_->skipChar();

    CAwkExpressionPtr expression;

    if (! parseExpression(&expression))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(')')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    if (isNewLine())
      parser_->skipChar();

    CAwkActionPtr action1;

    if (! parseStatement(&action1))
      return false;

    *action = CAwkWhileAction::create(expression, action1);
  }
  // input-output action
  else if (parseInputOutputAction(action)) {
  }
  // expression
  else {
    CAwkExpressionPtr expression;

    if (! parseExpression(&expression))
      return false;

    *action = CAwkExpressionAction::create(expression);
  }

  return true;
}

// <simple_statement>
bool
CAwk::
parseSimpleStatement(CAwkActionPtr *action)
{
  // printf <print_arg> '|' <terminal>
  // printf <print_arg> >> <terminal>
  // printf <print_arg> > <terminal>
  // printf <print_arg>
  if      (parser_->isWord("printf")) {
    parser_->skipChars(6);

    CAwkExpressionList expressionList;

    (void) parseRedirectExpressionList(expressionList);

    parser_->skipSpace();

    CAwkExpressionPtr fileExpr;

    CAwkOFilePtr file;

    if      (parser_->isChar('>')) {
      CAwkOFile::Type type = CAwkOFile::Type::WRITE_FILE;

      parser_->skipChar();

      if (parser_->isChar('>')) {
        parser_->skipChar();

        type = CAwkOFile::Type::APPEND_FILE;
      }

      if (! parseExpression(&fileExpr))
        return false;

      file = CAwkOFile::create(fileExpr, type);
    }
    else if (parser_->isChar('|')) {
      parser_->skipChar();

      if (! parseExpression(&fileExpr))
        return false;

      file = CAwkOFile::create(fileExpr, CAwkOFile::Type::PIPE_COMMAND);
    }

    *action = CAwkPrintFAction::create(expressionList, file);
  }
  // print <print_arg> '|' <terminal>
  // print <print_arg> >> <terminal>
  // print <print_arg> > <terminal>
  // print <print_arg>
  else if (parser_->isWord("print")) {
    parser_->skipChars(5);

    CAwkExpressionList expressionList;

    (void) parseRedirectExpressionList(expressionList);

    parser_->skipSpace();

    CAwkExpressionPtr fileExpr;

    CAwkOFilePtr file;

    if      (parser_->isChar('>')) {
      CAwkOFile::Type type = CAwkOFile::Type::WRITE_FILE;

      parser_->skipChar();

      if (parser_->isChar('>')) {
        parser_->skipChar();

        type = CAwkOFile::Type::APPEND_FILE;
      }

      if (! parseExpression(&fileExpr))
        return false;

      file = CAwkOFile::create(fileExpr, type);
    }
    else if (parser_->isChar('|')) {
      parser_->skipChar();

      if (! parseExpression(&fileExpr))
        return false;

      file = CAwkOFile::create(fileExpr, CAwkOFile::Type::PIPE_COMMAND);
    }

    *action = CAwkPrintAction::create(expressionList, file);
  }
  // delete <varname> '[' <pattern_list> ']'
  // delete <varname>
  else if (parser_->isWord("delete")) {
    parser_->skipChars(6);

    CAwkVariableRefPtr var;

    if (! parseVariable(&var))
      return false;

    CAwkExpressionPtr expression;

    parser_->skipSpace();

    if (parser_->isChar('[')) {
      if (! parseSubscript(&expression))
        return false;
    }

    *action = CAwkDeleteAction::create(var, expression);
  }
  // <pattern> ??
  else
    return false;

  return true;
}

bool
CAwk::
parseInputOutputAction(CAwkActionPtr *action)
{
  // close <term>
  if      (parser_->isWord("close")) {
    parser_->skipChars(5);

    parser_->skipSpace();

    if (! parser_->isChar('(')) {
      error("Expected '('");
      return false;
    }

    parser_->skipChar();

    CAwkExpressionPtr expression;

    if (! parseExpression(&expression))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(')')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    *action = CAwkCloseAction::create(expression);
  }
  // getline <var> < <term>
  // getline < <term>
  // getline <var>
  // getline
  else if (parser_->isWord("getline")) {
    parser_->skipChars(7);

    CAwkVariableRefPtr var;

    (void) parseVariable(&var);

    parser_->skipSpace();

    CAwkExpressionPtr filePtr;

    if (parser_->isChar('<')) {
      parser_->skipChar();

      if (! parseExpressionValue(&filePtr))
        return false;
    }

    CAwkIFilePtr file = CAwkIFile::create(filePtr, CAwkIFile::Type::READ_FILE);

    *action = CAwkGetLineAction::create(var, file);
  }
  // <simple_statement>
  else if (parseSimpleStatement(action)) {
  }
  else if (parser_->isWord("system")) {
    parser_->skipChars(6);

    parser_->skipSpace();

    if (! parser_->isChar('(')) {
      error("Expected '('");
      return false;
    }

    parser_->skipChar();

    CAwkExpressionPtr expr;

    if (! parseExpression(&expr))
      return false;

    parser_->skipSpace();

    if (! parser_->isChar(')')) {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    *action = CAwkSystemAction::create(expr);
  }
  else
    return false;

  return true;
}

bool
CAwk::
parseRedirectExpressionList(CAwkExpressionList &expressionList)
{
  int redirect_pos = ioFindRedirect();

  std::string save_line;

  if (redirect_pos >= 0) {
    int save_pos = parser_->getPos();

    save_line = parser_->getString();

    std::string new_line = save_line.substr(0, redirect_pos);

    parser_->setString(new_line);
    parser_->setPos   (save_pos);
  }

  bool rc = parseExpressionList(expressionList);

  if (redirect_pos >= 0) {
    int save_pos = parser_->getPos();

    parser_->setString(save_line);
    parser_->setPos   (save_pos);
  }

  return rc;
}

bool
CAwk::
parseExpressionList(CAwkExpressionList &expressionList)
{
  parser_->skipSpace();

  char c = parser_->getCharAt();

  if (c == ')')
    return true;

  if (c == '(') {
    parser_->skipChar();

    parseExpressionList(expressionList);

    parser_->skipSpace();

    char c = parser_->getCharAt();

    if (c != ')') {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();
  }
  else {
    CAwkExpressionPtr expression;

    if (! parseExpression(&expression))
      return false;

    expressionList.push_back(expression);

    parser_->skipSpace();

    char c = parser_->getCharAt();

    while (c == ',') {
      parser_->skipChar();

      if (isNewLine())
        parser_->skipChar();

      if (! parseExpression(&expression))
        return false;

      expressionList.push_back(expression);

      parser_->skipSpace();

      c = parser_->getCharAt();
    }
  }

  return true;
}

bool
CAwk::
parseExpression(CAwkExpressionPtr *expression)
{
  CAwkExpressionPtr expression1 = CAwkExpression::create();

  while (true) {
    parser_->skipSpace();

    if (parser_->eof())
      break;

    CAwkExpressionTermPtr term;

    if (! parseExpressionTerm(&term, expression1->isValue()))
      return false;

    if (! term.isValid())
      break;

    expression1->pushTerm(term);
  }

  if (expression1->numTerms() == 0)
    return false;

  *expression = expression1;

  if (getDebug())
    std::cerr << *expression << std::endl;

  return true;
}

bool
CAwk::
parseExpressionValue(CAwkExpressionPtr *expression)
{
  CAwkExpressionPtr expression1 = CAwkExpression::create();

  while (true) {
    parser_->skipSpace();

    if (parser_->eof())
      break;

    CAwkExpressionTermPtr term;

    if (! parseExpressionTerm(&term, expression1->isValue()))
      return false;

    if (! term.isValid())
      break;

    expression1->pushTerm(term);

    if (expression1->isValue())
      break;
  }

  if (expression1->numTerms() == 0)
    return false;

  *expression = expression1;

  if (getDebug())
    std::cerr << *expression << std::endl;

  return true;
}

bool
CAwk::
parseExpressionTerm(CAwkExpressionTermPtr *term, bool isValue)
{
  char c = parser_->getCharAt();

  // <string>
  if      (c == '"') {
    CAwkValuePtr value;

    if (! parseString(&value))
      return false;

    *term = value.refCast<CAwkExpressionTerm>();
  }
  // <number>
  else if (isdigit(c) || c == '.') {
    CAwkValuePtr real;

    if (! parseNumber(&real))
      return false;

    *term = real.refCast<CAwkExpressionTerm>();
  }
  // ( <pattern> )
  else if (c == '(') {
    parser_->skipChar();

    CAwkExpressionPtr expression2;

    if (! parseExpression(&expression2))
      return false;

    parser_->skipSpace();

    c = parser_->getCharAt();

    if (c != ')') {
      error("Expected ')'");
      return false;
    }

    parser_->skipChar();

    *term = expression2.refCast<CAwkExpressionTerm>();
  }
  else if (c == '/' && ! isValue) {
    std::string regexp;

    if (! parseRegularExpression(regexp))
      return false;

    // TODO: regexp value ?
    CAwkValuePtr value = CAwkValue::create(regexp);

    *term = value.refCast<CAwkExpressionTerm>();
  }
  else if (parser_->isWord("getline")) {
    parser_->skipChars(7);

    CAwkVariableRefPtr var;

    (void) parseVariable(&var);

    parser_->skipSpace();

    CAwkExpressionPtr filePtr;

    if (parser_->isChar('<')) {
      parser_->skipChar();

      if (! parseExpressionValue(&filePtr))
        return false;
    }

    CAwkIFilePtr file = CAwkIFile::create(filePtr, CAwkIFile::Type::READ_FILE);

    CAwkExpressionTermPtr expr = CAwkGetLineExpr::create(var, file);

    *term = expr;
  }
  // <ppattern> ~ <regexp>
  // <ppattern> ~ <ppattern>
  else if (parser_->isString("!~")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op = CAwkNotRegExpOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <ppattern> !~ <regexp>
  // <ppattern> !~ <ppattern>
  else if (parser_->isString("~")) {
    parser_->skipChar();

    CAwkOperatorPtr op = CAwkRegExpOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }

  // <ppattern> ? <ppattern> : <ppattern>
  else if (isValue && strchr("?:", c) != 0) {
    parser_->skipChar();

    CAwkOperatorPtr op;

    if (c == '?')
      op = CAwkQuestionOperator::create();
    else
      op = CAwkColonOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <ppattern> || <ppattern>
  else if (parser_->isString("||")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op = CAwkLogicalOrOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <ppattern> && <ppattern>
  else if (parser_->isString("&&")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op = CAwkLogicalAndOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <pattern == pattern
  else if (parser_->isString("==")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op = CAwkEqualsOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <pattern >= pattern
  else if (parser_->isString(">=")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op = CAwkGreaterEqualsOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <pattern >  pattern
  else if (parser_->isString(">")) {
    parser_->skipChar();

    CAwkOperatorPtr op = CAwkGreaterOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <pattern <= pattern
  else if (parser_->isString("<=")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op = CAwkLessEqualsOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <pattern <  pattern
  else if (parser_->isString("<")) {
    parser_->skipChar();

    CAwkOperatorPtr op = CAwkLessOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <pattern != pattern
  else if (parser_->isString("!=")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op = CAwkNotEqualsOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <ppattern> in <varname>
  // ( <pattern_list> ) in <varname>
  else if (parser_->isWord("in")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op = CAwkInOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }

  // <ppattern> <term>
  // <regexp>
  // ! <regexp>

  // <term>

  // ++ <var>
  // <var> ++
  else if (parser_->isString("++")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op;

    if (isValue)
      op = CAwkPostIncrementOperator::create();
    else
      op = CAwkPreIncrementOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <var> --
  // -- <var>
  else if (parser_->isString("--")) {
    parser_->skipChars(2);

    CAwkOperatorPtr op;

    if (isValue)
      op = CAwkPostDecrementOperator::create();
    else
      op = CAwkPreDecrementOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // <term> = <term>
  // <term> += <term>
  // <term> -= <term>
  // <term> *= <term>
  // <term> /= <term>
  // <term> %= <term>
  // <term> ^= <term>
  // <term> + <term>
  // <term> - <term>
  // <term> * <term>
  // <term> / <term>
  // <term> % <term>
  // <term> ^ <term>
  else if (isValue && strchr("=+-*/%^", c) != 0) {
    CAwkOperatorPtr op;

    if (! parseOperator(&op, isValue))
      return false;

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // + <term>
  // - <term>
  // ! <term>
  else if (! isValue && strchr("+-!", c) != 0) {
    CAwkOperatorPtr op;

    if (! parseOperator(&op, isValue))
      return false;

    *term = op.refCast<CAwkExpressionTerm>();
  }
  // $ <term>
  else if (c == '$') {
    parser_->skipChar();

    CAwkOperatorPtr op = CAwkFieldOperator::create();

    *term = op.refCast<CAwkExpressionTerm>();
  }

  // <builtin> ( )
  // <builtin> ( <pattern_list> )
  // <builtin>
  else if (isalpha(c) || c == '_') {
    std::string name;

    if (! parseIdentifier(name))
      return false;

    parser_->skipSpace();

    if      (parser_->isChar('(')) {
      parser_->skipChar();

      CAwkExpressionList expressionList;

      if (! parseExpressionList(expressionList))
        return false;

      parser_->skipSpace();

      if (! parser_->isChar(')')) {
        error("Expected ')'");
        return false;
      }

      parser_->skipChar();

      CAwkExpressionTermPtr function =
        CAwkExprFunction::create(name, expressionList);

      *term = function.refCast<CAwkExpressionTerm>();
    }
    else if (parser_->isChar('[')) {
      parser_->skipChar();

      CAwkExpressionList expressionList;

      if (! parseExpressionList(expressionList))
        return false;

      parser_->skipSpace();

      if (! parser_->isChar(']')) {
        error("Expected ']'");
        return false;
      }

      parser_->skipChar();

      CAwkVariableRefPtr var =
        CAwkArrayVariableRef::create(name, expressionList);

      *term = var.refCast<CAwkExpressionTerm>();
    }
    else {
      CAwkVariableRefPtr var = CAwkVariableRef::create(name);

      *term = var.refCast<CAwkExpressionTerm>();
    }
  }
  else {
    *term = CAwkExpressionTermPtr();
    return true;
  }

  // <term> | getline <var>
  if ((*term)->hasValue()) {
    parser_->skipSpace();

    if (parser_->isChar('|')) {
      int save_pos = parser_->getPos();

      parser_->skipChar();

      parser_->skipSpace();

      if (parser_->isWord("getline")) {
        parser_->skipChars(7);

        CAwkVariableRefPtr var;

        (void) parseVariable(&var);

        parser_->skipSpace();

        CAwkExpressionPtr filePtr = CAwkExpression::create();

        filePtr->pushTerm(*term);

        CAwkIFilePtr file = CAwkIFile::create(filePtr, CAwkIFile::Type::PIPE_COMMAND);

        *term = CAwkGetLineExpr::create(var, file);
      }
      else
        parser_->setPos(save_pos);
    }
  }

  return true;
}

bool
CAwk::
parseRegularExpression(std::string &regexp)
{
  if (! parser_->isChar('/'))
    return false;

  parser_->skipChar();

  while (! parser_->eof() && ! parser_->isChar('/')) {
    regexp += parser_->getCharAt();

    parser_->skipChar();
  }

  if (! parser_->isChar('/')) {
    error("Expected '/'");
    return false;
  }

  parser_->skipChar();

  return true;
}

bool
CAwk::
parseSubscript(CAwkExpressionPtr *expression)
{
  if (! parser_->isChar('['))
    return false;

  parser_->skipChar();

  if (! parseExpression(expression))
    return false;

  parser_->skipSpace();

  if (! parser_->isChar(']')) {
    error("Expected ']'");
    return false;
  }

  parser_->skipChar();

  return true;
}

// <varname> [ <pattern_list> ]

bool
CAwk::
parseVariable(CAwkVariableRefPtr *var)
{
  std::string name;

  if (! parseIdentifier(name))
    return false;

  *var = CAwkVariableRef::create(name);

  if (getDebug())
    std::cerr << *var << std::endl;

  return true;
}

// <varname>
bool
CAwk::
parseIdentifier(std::string &name)
{
  parser_->skipSpace();

  char c = parser_->getCharAt();

  if (! isalpha(c) && c != '_')
    return false;

  name += c;

  parser_->skipChar();

  c = parser_->getCharAt();

  while (! parser_->eof() &&
         (isalnum(c) || c == '_')) {
    name += c;

    parser_->skipChar();

    c = parser_->getCharAt();
  }

  return true;
}

bool
CAwk::
parseString(CAwkValuePtr *value)
{
  std::string str;

  if (! parseString(str))
    return false;

  *value = CAwkValue::create(str);

  return true;
}

bool
CAwk::
parseString(std::string &str)
{
  parser_->skipSpace();

  char c = parser_->getCharAt();

  if (c == '"') {
    std::string str1;

    if (! parser_->readString(str1)) {
      error("Expected '\"'");
      return false;
    }

    // remove quotes
    str = str1.substr(1, str1.size() - 2);
  }
  else
    return false;

  str = CStrUtil::replaceEscapeCodes(str);

  return true;
}

bool
CAwk::
parseNumber(CAwkValuePtr *value)
{
  char c = parser_->getCharAt();

  if (isdigit(c)) {
    int integer;

    int save_pos = parser_->getPos();

    if (! parser_->readInteger(&integer))
      return false;

    char c = parser_->getCharAt();

    if (c == '.' || c == 'E') {
      parser_->setPos(save_pos);

      double real;

      if (!  parser_->readReal(&real))
        return false;

      *value = CAwkValue::create(real);
    }
    else
      *value = CAwkValue::create(integer);
  }
  else {
    double real;

    if (! parser_->readReal(&real))
      return false;

    *value = CAwkValue::create(real);
  }

  return true;
}

bool
CAwk::
parseOperator(CAwkOperatorPtr *op, bool value)
{
  parser_->skipSpace();

  char c = parser_->getCharAt();

  if      (c == '=') {
    parser_->skipChar();

    *op = CAwkAssignOperator::create();
  }
  else if (c == '+') {
    parser_->skipChar();

    c = parser_->getCharAt();

    if      (c == '=') {
      parser_->skipChar();

      *op = CAwkPlusEqualsOperator::create();
    }
    else {
      if (value)
        *op = CAwkPlusOperator::create();
      else
        *op = CAwkUnaryPlusOperator::create();
    }
  }
  else if (c == '-') {
    parser_->skipChar();

    c = parser_->getCharAt();

    if      (c == '=') {
      parser_->skipChar();

      *op = CAwkMinusEqualsOperator::create();
    }
    else {
      if (value)
        *op = CAwkMinusOperator::create();
      else
        *op = CAwkUnaryMinusOperator::create();
    }
  }
  else if (c == '*') {
    parser_->skipChar();

    c = parser_->getCharAt();

    if      (c == '=') {
      parser_->skipChar();

      *op = CAwkTimesEqualsOperator::create();
    }
    else
      *op = CAwkTimesOperator::create();
  }
  else if (c == '/') {
    parser_->skipChar();

    c = parser_->getCharAt();

    if      (c == '=') {
      parser_->skipChar();

      *op = CAwkDivideEqualsOperator::create();
    }
    else
      *op = CAwkDivideOperator::create();
  }
  else if (c == '%') {
    parser_->skipChar();

    c = parser_->getCharAt();

    if      (c == '=') {
      parser_->skipChar();

      *op = CAwkModulusEqualsOperator::create();
    }
    else
      *op = CAwkModulusOperator::create();
  }
  else if (c == '^') {
    parser_->skipChar();

    c = parser_->getCharAt();

    if      (c == '=') {
      parser_->skipChar();

      *op = CAwkPowerEqualsOperator::create();
    }
    else
      *op = CAwkPowerOperator::create();
  }
  else if (c == '!') {
    parser_->skipChar();

    *op = CAwkLogicalNotOperator::create();
  }
  else if (c == '$') {
    parser_->skipChar();

    *op = CAwkFieldOperator::create();
  }
  else
    return false;

  return true;
}

int
CAwk::
ioFindRedirect()
{
  int in_brackets = 0;

  int save_pos = parser_->getPos();

  while (! parser_->eof()) {
    char c = parser_->getCharAt();

    if      (c == '(') {
      ++in_brackets;

      parser_->skipChar();
    }
    else if (c == ')') {
      --in_brackets;

      parser_->skipChar();
    }
    else if (c == '"') {
      parser_->skipString();

      parser_->skipChar();
    }
    else if (c == '>') {
      if (in_brackets == 0) {
        int pos = parser_->getPos();

        parser_->setPos(save_pos);

        return pos;
      }

      parser_->skipChar();
    }
    else
      parser_->skipChar();
  }

  parser_->setPos(save_pos);

  return -1;
}

bool
CAwk::
isNewLine()
{
  if (parser_->eof()) {
    std::string line;

    if (! readLine(line))
      return false;

    int save_pos = parser_->getPos();

    std::string current_line = parser_->getString();

    current_line += "#" + line;

    parser_->setString(current_line);
    parser_->setPos   (save_pos);

    return true;
  }

  return parser_->isChar('#');
}

bool
CAwk::
readLine(std::string &line)
{
  if (! input_file_->readLine(line))
    return false;

  line = removeComments(line);

  line = CStrUtil::stripSpaces(line);

  ++line_num_;

  uint len = line.size();

  if (len > 0 && line[len - 1] == '\\') {
    line = line.substr(0, len - 1);

    std::string line1;

    (void) readLine(line1);

    line += line1;
  }

  return true;
}

std::string
CAwk::
removeComments(const std::string &line)
{
  std::string line1;

  uint len = line.size();

  uint i = 0;

  while (i < len) {
    // skip string
    if      (line[i] == '\"') {
      uint j = i;

      if (! CStrUtil::skipDoubleQuotedString(line, &i))
        return "";

      line1 += line.substr(j, i - j);
    }
    // skip regexp
    else if (line[i] == '/') {
      uint j = i;

      ++i;

      while (i < len && line[i] != '/') {
        if (line[i] == '\\')
          ++i;

        ++i;
      }

      if (line[i] == '/')
        ++i;

      line1 += line.substr(j, i - j);
    }
    else if (line[i] == '#')
      break;
    else
      line1 += line[i++];
  }

  return line1;
}

CAwkValuePtr
CAwk::
getValue(CAwkExpressionTermPtr term)
{
  if      (term.canCast<CAwkValue>())
    return term.refCast<CAwkValue>();
  else if (term->hasValue())
    return term->getValue();
  else
    return CAwkValuePtr();
}

CAwkVariableRefPtr
CAwk::
getVariableRef(CAwkExpressionTermPtr term)
{
  CAwkExpressionTermPtr term1 = term->execute();

  if (term1.canCast<CAwkVariableRef>())
    return term1.refCast<CAwkVariableRef>();
  else
    return CAwkVariableRefPtr();
}

CAwkVariablePtr
CAwk::
getVariable(const std::string &name, bool create) const
{
  CAwkVariablePtr variable;

  if (currentBlock_.isValid()) {
    variable = currentBlock_->getVariable(name);

    if (variable.isValid())
      return variable;

    CAwkActionBlockList::const_reverse_iterator p1 = blockStack_.rbegin();
    CAwkActionBlockList::const_reverse_iterator p2 = blockStack_.rend  ();

    for ( ; p1 != p2; ++p1) {
      variable = (*p1)->getVariable(name);

      if (variable.isValid())
        return variable;
    }
  }

  variable = variableMgr_.getVariable(name);

  if (variable.isValid())
    return variable;

  if (create) {
    CAwk *th = const_cast<CAwk *>(this);

    return th->addVariable(name);
  }

  return CAwkVariablePtr();
}

CAwkVariablePtr
CAwk::
addVariable(const std::string &name)
{
  if (currentBlock_.isValid())
    return currentBlock_->addVariable(name);
  else {
    CAwkVariablePtr variable = CAwkVariable::create(name, "");

    variableMgr_.addVariable(variable);

    return variable;
  }
}

CFile *
CAwk::
getFile(FILE *file)
{
  return fileMgr_.getFile(file);
}

CFile *
CAwk::
getFile(const std::string &fileName, CFileBase::Mode mode)
{
  return fileMgr_.getFile(fileName, mode);
}

bool
CAwk::
closeFile(const std::string &fileName)
{
  return fileMgr_.closeFile(fileName);
}

CAwkPipe *
CAwk::
getPipe(const std::string &cmdName, CAwkPipe::Type type)
{
  return pipeMgr_.getPipe(cmdName, type);
}

bool
CAwk::
closePipe(const std::string &cmdName)
{
  return pipeMgr_.closePipe(cmdName);
}

void
CAwk::
startBlock(CAwkActionBlockPtr block)
{
  if (currentBlock_.isValid())
    blockStack_.push_back(currentBlock_);

  currentBlock_ = block;
}

void
CAwk::
endBlock()
{
  if (! blockStack_.empty()) {
    currentBlock_ = blockStack_.back();

    blockStack_.pop_back();
  }
  else
    currentBlock_ = CAwkActionBlockPtr();
}

void
CAwk::
error(const std::string &str) const
{
  std::string lhs = parser_->getString().substr(0, parser_->getPos());
  std::string rhs = parser_->getString().substr(parser_->getPos());

  std::cerr << file_name_ << "@" << line_num_ << "," << parser_->getPos() << std::endl;
  std::cerr << lhs << '^' << rhs << std::endl;
  std::cerr << str << std::endl;
}
