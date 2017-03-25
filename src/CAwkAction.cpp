#include <CAwk.h>
#include <CPrintF.h>
#include <CFuncs.h>
#include <CCommand.h>
#include <CStrUtil.h>
#include <CReadLine.h>
#include <cstdio>

void
CAwkNullAction::
exec()
{
  std::string str = CAwkInst->getLineField(0);

  std::cout << str;

  std::cout << CAwkInst->getVariable("ORS")->getValue()->getString();
}

//-----------

void
CAwkBreakAction::
exec()
{
  CAwkInst->setBreakFlag();
}

//-----------

void
CAwkContinueAction::
exec()
{
  CAwkInst->setContinueFlag();
}

//-----------

void
CAwkNextAction::
exec()
{
  CAwkInst->setNextFlag();
}

//-----------

void
CAwkReturnAction::
exec()
{
  CAwkValuePtr value = expression_->getValue();

  CAwkInst->setReturnValue(value);
}

void
CAwkReturnAction::
print(std::ostream &os) const
{
  os << "return";

  if (expression_.isValid())
    os << " " << *expression_;
}

//-----------

void
CAwkExitAction::
exec()
{
  CAwkInst->setExitFlag();
}

void
CAwkExitAction::
print(std::ostream &os) const
{
  os << "exit";

  if (expression_.isValid())
    os << " " << *expression_;
}

//----------

void
CAwkDeleteAction::
exec()
{
  std::string ind = expression_->getValue()->getString();

  var_->removeInd(ind);
}

void
CAwkDeleteAction::
print(std::ostream &os) const
{
  os << "delete " << *var_;

  if (expression_.isValid())
    os << "[" << *expression_ << "]";
}

//----------

CAwkIfAction::
CAwkIfAction(CAwkExpressionPtr expression, CAwkActionPtr action) :
 expression_(expression)
{
  actionList_ = CAwkActionList::create(CAwkActionList::Type::SIMPLE);

  actionList_->addAction(action);
}

void
CAwkIfAction::
exec()
{
  CAwkValuePtr value = expression_->getValue();

  if (value->getBool())
    actionList_->exec();
}

void
CAwkIfAction::
print(std::ostream &os) const
{
  os << "if (" << *expression_ << ") " << *actionList_;
}

//----------

CAwkIfElseAction::
CAwkIfElseAction(CAwkExpressionPtr expression,
                 CAwkActionPtr action1, CAwkActionPtr action2) :
 expression_(expression)
{
  actionList1_ = CAwkActionList::create(CAwkActionList::Type::SIMPLE);
  actionList2_ = CAwkActionList::create(CAwkActionList::Type::SIMPLE);

  actionList1_->addAction(action1);
  actionList2_->addAction(action2);
}

void
CAwkIfElseAction::
exec()
{
  CAwkValuePtr value = expression_->getValue();

  if (value->getBool())
    actionList1_->exec();
  else
    actionList2_->exec();
}

void
CAwkIfElseAction::
print(std::ostream &os) const
{
  os << "if (" << *expression_ << ") " <<
        *actionList1_ << " else " << *actionList2_;
}

//----------

CAwkForAction::
CAwkForAction(CAwkExpressionPtr expression1, CAwkExpressionPtr expression2,
              CAwkExpressionPtr expression3, CAwkActionPtr action) :
 expression1_(expression1), expression2_(expression2),
 expression3_(expression3)
{
  actionList_ = CAwkActionList::create(CAwkActionList::Type::SIMPLE);

  actionList_->addAction(action);
}

void
CAwkForAction::
exec()
{
  (void) expression1_->execute();

  CAwkValuePtr value = expression2_->getValue();

  while (value->getBool()) {
    actionList_->exec();

    if (CAwkInst->isBreakFlag()) {
      CAwkInst->resetBreakFlag();
      break;
    }

    if (CAwkInst->isContinueFlag())
      CAwkInst->resetContinueFlag();

    if (CAwkInst->isExitFlag())
      return;

    //----

    (void) expression3_->execute();

    value = expression2_->getValue();
  }
}

void
CAwkForAction::
print(std::ostream &os) const
{
  os << "for (" << *expression1_ << "; " << *expression2_ << "; " <<
        *expression3_ << ") " << *actionList_;
}

//----------

CAwkForInAction::
CAwkForInAction(CAwkVariableRefPtr var1, CAwkVariableRefPtr var2,
                CAwkActionPtr action) :
 var1_(var1), var2_(var2)
{
  actionList_ = CAwkActionList::create(CAwkActionList::Type::SIMPLE);

  actionList_->addAction(action);
}

void
CAwkForInAction::
exec()
{
  StringVectorT indices = var2_->getIndices();

  StringVectorT::const_iterator p1 = indices.begin();
  StringVectorT::const_iterator p2 = indices.end  ();

  for ( ; p1 != p2; ++p1) {
    var1_->setValue(CAwkValue::create(*p1));

    //----

    actionList_->exec();

    if (CAwkInst->isBreakFlag()) {
      CAwkInst->resetBreakFlag();
      break;
    }

    if (CAwkInst->isContinueFlag())
      CAwkInst->resetContinueFlag();

    if (CAwkInst->isExitFlag())
      return;
  }
}

void
CAwkForInAction::
print(std::ostream &os) const
{
  os << "for (" << *var1_ << " in " << *var2_ << ") " << *actionList_;
}

//----------

CAwkWhileAction::
CAwkWhileAction(CAwkExpressionPtr expression, CAwkActionPtr action) :
 expression_(expression)
{
  actionList_ = CAwkActionList::create(CAwkActionList::Type::SIMPLE);

  actionList_->addAction(action);
}

void
CAwkWhileAction::
exec()
{
  CAwkValuePtr value = expression_->getValue();

  while (value->getBool()) {
    actionList_->exec();

    if (CAwkInst->isBreakFlag()) {
      CAwkInst->resetBreakFlag();
      break;
    }

    if (CAwkInst->isContinueFlag())
      CAwkInst->resetContinueFlag();

    if (CAwkInst->isExitFlag())
      return;

    //----

    value = expression_->getValue();
  }
}

void
CAwkWhileAction::
print(std::ostream &os) const
{
  os << "while (" << *expression_ << ") " << *actionList_;
}

//----------

CAwkDoWhileAction::
CAwkDoWhileAction(CAwkActionPtr action, CAwkExpressionPtr expression) :
 expression_(expression)
{
  actionList_ = CAwkActionList::create(CAwkActionList::Type::SIMPLE);

  actionList_->addAction(action);
}

void
CAwkDoWhileAction::
exec()
{
  CAwkValuePtr value;

  do {
    actionList_->exec();

    if (CAwkInst->isBreakFlag()) {
      CAwkInst->resetBreakFlag();
      break;
    }

    if (CAwkInst->isContinueFlag())
      CAwkInst->resetContinueFlag();

    if (CAwkInst->isExitFlag())
      return;

    //----

    value = expression_->getValue();
  }
  while (value->getBool());
}

void
CAwkDoWhileAction::
print(std::ostream &os) const
{
  os << "do " << *actionList_ << " while (" << *expression_ << ")";
}

//-----------

void
CAwkActionListAction::
exec()
{
  actionList_->exec();
}

void
CAwkActionListAction::
print(std::ostream &os) const
{
  actionList_->print(os);
}

//-----------

void
CAwkExpressionAction::
exec()
{
  /*CAwkValuePtr value =*/ expression_->execute();

  //std::cout << *value << std::endl;
}

void
CAwkExpressionAction::
print(std::ostream &os) const
{
  os << *expression_;
}

//-----------

void
CAwkCloseAction::
exec()
{
  std::string str = expression_->getValue()->getString();

  CAwkInst->closeFile(str);
  CAwkInst->closePipe(str);
}

void
CAwkCloseAction::
print(std::ostream &os) const
{
  os << "close(" << *expression_ << ")";
}

//-----------

void
CAwkGetLineAction::
exec()
{
  std::string line;

  if (file_.isValid()) {
    std::string str;

    file_->read(line);
  }
  else {
    CReadLine readline;

    line = readline.readLine();
  }

  var_->setValue(CAwkValue::create(line));
}

void
CAwkGetLineAction::
print(std::ostream &os) const
{
  os << "getline";

  if (var_.isValid())
    os << " " << *var_;

  if (file_.isValid())
    os << " " << *file_;
}

//-----------

void
CAwkPrintAction::
exec()
{
  std::string str;

  if (expressionList_.empty())
    str = CAwkInst->getLineField(0);
  else {
    CAwkExpressionList::iterator p1 = expressionList_.begin();
    CAwkExpressionList::iterator p2 = expressionList_.end  ();

    for (int i = 0; p1 != p2; ++p1, ++i) {
      if (i > 0)
        str += CAwkInst->getVariable("OFS")->getValue()->getString();

      CAwkValuePtr value = (*p1)->getValue();

      if (value.isValid())
        str += value->getString();
    }
  }

  str += CAwkInst->getVariable("ORS")->getValue()->getString();

  if (file_.isValid())
    file_->write(str);
  else
    std::cout << str;
}

void
CAwkPrintAction::
print(std::ostream &os) const
{
  os << "print";

  if (! expressionList_.empty()) {
    os << " ";

    for_each(expressionList_.begin(), expressionList_.end(),
             CPrintSeparated<CAwkExpressionPtr>(os));
  }

  if (file_.isValid())
    os << *file_;
}

//-----------

void
CAwkPrintFAction::
exec()
{
  if (expressionList_.empty())
    return;

  CAwkExpressionList::iterator p1 = expressionList_.begin();
  CAwkExpressionList::iterator p2 = expressionList_.end  ();

  CAwkValuePtr value = (*p1)->getValue();

  ++p1;

  if (! value.isValid())
    return;

  class PrintF : public CPrintF {
   private:
    mutable CAwkExpressionList::iterator p1_;
    mutable CAwkExpressionList::iterator p2_;

   public:
    PrintF(const std::string &format, CAwkExpressionList::iterator p1,
           CAwkExpressionList::iterator p2) :
     CPrintF(format), p1_(p1), p2_(p2) {
    }

    int         getInt     () const { return getValue()->getInteger(); }
    long        getLong    () const { return getValue()->getInteger(); }
    long        getLongLong() const { return getValue()->getInteger(); }
    double      getDouble  () const { return getValue()->getReal   (); }
    std::string getString  () const { return getValue()->getString (); }

    CAwkValuePtr getValue() const {
      CAwkValuePtr value;

      if (p1_ != p2_) {
        value = (*p1_)->getValue();

        ++p1_;
      }
      else
        value = CAwkValue::create("");

      return value;
    }
  };

  PrintF printf(value->getString(), p1, p2);

  std::string str = printf.format();

  if (file_.isValid())
    file_->write(str);
  else
    std::cout << str;
}

void
CAwkPrintFAction::
print(std::ostream &os) const
{
  os << "printf ";

  for_each(expressionList_.begin(), expressionList_.end(),
           CPrintSeparated<CAwkExpressionPtr>(os));

  if (file_.isValid())
    os << *file_;
}

//-----------

void
CAwkSystemAction::
exec()
{
  CAwkValuePtr value = expr_->getValue();

  (void) system(value->getString().c_str());

  //std::cout << *value << std::endl;
}

void
CAwkSystemAction::
print(std::ostream &os) const
{
  os << "system(" << *expr_ << ")";
}

//-----------

void
CAwkActionList::
addAction(CAwkActionPtr action)
{
  actionList_.push_back(action);
}

void
CAwkActionList::
exec()
{
  // TODO: stack of action lists
  ActionList::iterator p1 = actionList_.begin();
  ActionList::iterator p2 = actionList_.end  ();

  for ( ; p1 != p2; ++p1) {
    if      (CAwkInst->isBreakFlag()) {
      if (type_ == Type::PROGRAM || type_ == Type::ROUTINE)
        CAwkInst->error("break not in iteration");

      if (type_ == Type::SIMPLE || type_ == Type::ITERATION)
        break;
    }
    else if (CAwkInst->isContinueFlag()) {
      if (type_ == Type::PROGRAM || type_ == Type::ROUTINE)
        CAwkInst->error("continue not in iteration");

      if (type_ == Type::SIMPLE)
        break;
    }
    else if (CAwkInst->isNextFlag())
      break;
    else if (CAwkInst->isReturnFlag())
      break;
    else if (CAwkInst->isExitFlag())
      break;

    (*p1)->exec();
  }

  if (CAwkInst->isBreakFlag()) {
    if (type_ == Type::PROGRAM || type_ == Type::ROUTINE)
      CAwkInst->error("break not in iteration");

    if (type_ == Type::ITERATION)
      CAwkInst->resetBreakFlag();
  }
  else if (CAwkInst->isContinueFlag()) {
    if (type_ == Type::PROGRAM || type_ == Type::ROUTINE)
      CAwkInst->error("continue not in iteration");

    if (type_ == Type::ITERATION)
      CAwkInst->resetContinueFlag();
  }
  else if (CAwkInst->isNextFlag()) {
    if (type_ == Type::PROGRAM)
      CAwkInst->resetNextFlag();
  }
  else if (CAwkInst->isReturnFlag()) {
    if (type_ == Type::PROGRAM || type_ == Type::ROUTINE)
      CAwkInst->resetReturnFlag();
  }
}

void
CAwkActionList::
print(std::ostream &os) const
{
  for_each(actionList_.begin(), actionList_.end(),
           CPrintSeparated<CAwkActionPtr>(os, ";"));
}

//-----------

CAwkVariablePtr
CAwkActionBlock::
getVariable(const std::string &name) const
{
  return variableMgr_.getVariable(name);
}

CAwkVariablePtr
CAwkActionBlock::
addVariable(const std::string &name)
{
  CAwkVariablePtr variable = CAwkVariable::create(name, "");

  variableMgr_.addVariable(variable);

  return variable;
}

void
CAwkActionBlock::
exec()
{
  actionList_->exec();
}

void
CAwkActionBlock::
print(std::ostream &os) const
{
  os << "vars {";

  variableMgr_.print(os);

  os << "} {";

  actionList_->print(os);

  os << "}";
}

//-----------

bool
CAwkPatternAction::
isBegin() const
{
  return (pattern_.canCast<CAwkBeginPattern>());
}

bool
CAwkPatternAction::
isEnd() const
{
  return (pattern_.canCast<CAwkEndPattern>());
}

void
CAwkPatternAction::
exec()
{
  if (pattern_->exec())
    actionList_->exec();
}

void
CAwkPatternAction::
print(std::ostream &os) const
{
  os << *pattern_ << " { ";

  actionList_->print(os);

  os << " }";
}

//---------------

CAwkFileMgr::
CAwkFileMgr() :
 stdInFile_(0), stdOutFile_(0), stdErrFile_(0)
{
}

void
CAwkFileMgr::
init()
{
  term();
}

void
CAwkFileMgr::
term()
{
  delete stdInFile_ ; stdInFile_  = 0;
  delete stdOutFile_; stdOutFile_ = 0;
  delete stdErrFile_; stdErrFile_ = 0;

  FileList::iterator p1 = files_.begin();
  FileList::iterator p2 = files_.end  ();

  for ( ; p1 != p2; ++p1)
    delete *p1;

  files_.clear();
}

CFile *
CAwkFileMgr::
getFile(FILE *file)
{
  if      (file == stdin) {
    if (! stdInFile_)
      stdInFile_ = new CFile(stdin);

    return stdInFile_;
  }
  else if (file == stdout) {
    if (! stdOutFile_)
      stdOutFile_ = new CFile(stdout);

    return stdOutFile_;
  }
  else if (file == stderr) {
    if (! stdErrFile_)
      stdErrFile_ = new CFile(stderr);

    return stdErrFile_;
  }
  else
    assert(false);
}

CFile *
CAwkFileMgr::
getFile(const std::string &fileName, CFileBase::Mode mode)
{
  CFile *file = new CFile(fileName);

  std::string path = file->getPath();

  FileList::iterator p1 = files_.begin();
  FileList::iterator p2 = files_.end  ();

  for ( ; p1 != p2; ++p1)
    if ((*p1)->getPath() == path)
      break;

  if (p1 != p2) {
    delete file;

    file = *p1;

    CFileBase::Mode mode1 = file->getOpenMode();

    if (mode != mode1)
      file->open(mode);
  }
  else {
    files_.push_back(file);

    file->open(mode);
  }

  return file;
}

bool
CAwkFileMgr::
closeFile(const std::string &fileName)
{
  CFile file(fileName);

  std::string path = file.getPath();

  FileList::iterator p1 = files_.begin();
  FileList::iterator p2 = files_.end  ();

  for ( ; p1 != p2; ++p1) {
    if ((*p1)->getPath() == path) {
      files_.erase(p1);
      return true;
    }
  }

  return false;
}

//--------------

CAwkPipeMgr::
~CAwkPipeMgr()
{
  term();
}

void
CAwkPipeMgr::
init()
{
  term();
}

void
CAwkPipeMgr::
term()
{
  PipeList::iterator p1 = pipes_.begin();
  PipeList::iterator p2 = pipes_.end  ();

  for ( ; p1 != p2; ++p1)
    delete *p1;

  pipes_.clear();
}

CAwkPipe *
CAwkPipeMgr::
getPipe(const std::string &cmdName, CAwkPipe::Type type)
{
  PipeList::iterator p1 = pipes_.begin();
  PipeList::iterator p2 = pipes_.end  ();

  for ( ; p1 != p2; ++p1)
    if ((*p1)->getCmdName() == cmdName)
      break;

  if (p1 != p2) {
    if ((*p1)->getType() == type)
      return *p1;

    pipes_.erase(p1);
  }

  CAwkPipe *pipe = new CAwkPipe(cmdName, type);

  pipes_.push_back(pipe);

  return pipe;
}

bool
CAwkPipeMgr::
closePipe(const std::string &cmdName)
{
  PipeList::iterator p1 = pipes_.begin();
  PipeList::iterator p2 = pipes_.end  ();

  for ( ; p1 != p2; ++p1) {
    if ((*p1)->getCmdName() == cmdName) {
      (*p1)->close();
      pipes_.erase(p1);
      return true;
    }
  }

  return false;
}

CAwkPipe::
CAwkPipe(const std::string &cmdName, Type type) :
 cmdName_(cmdName), type_(type), opened_(true)
{
}

CAwkPipe::
~CAwkPipe()
{
  if (opened_)
    close();
}

void
CAwkPipe::
close()
{
  std::vector<std::string> args;

  args.push_back("-c");
  args.push_back(cmdName_);

  CCommand command("sh", "/bin/sh", args);

  std::string res;

  if      (type_ == Type::INPUT)
    command.addStringDest(*output_);
  else if (type_ == Type::OUTPUT) {
    command.addStringSrc (input_);
    command.addStringDest(res);
  }

  command.start();

  command.wait();

  if (type_ == Type::OUTPUT)
    std::cout << res;

  opened_ = false;
}

//--------------

bool
CAwkIFile::
read(std::string &str) const
{
  if      (type_ == Type::READ_FILE) {
    CFile *file = getFile();

    char buffer[256];

    size_t numRead = 0;

    while (file->read((uchar *) buffer, 255, &numRead)) {
      buffer[numRead] = '\0';

      str += buffer;
    }

    return (str.size() > 0);
  }
  else if (type_ == Type::PIPE_COMMAND) {
    std::string cmdStr = file_->getValue()->getString();

    CAwkPipe *pipe = CAwkInst->getPipe(cmdStr, CAwkPipe::Type::INPUT);

    pipe->setOutput(&str);

    CAwkInst->closePipe(cmdStr);

    return (str.size() > 0);
  }
  else
    assert(false);
}

CFile *
CAwkIFile::
getFile() const
{
  if (file_.isValid()) {
    std::string fileName = file_->getValue()->getString();

    return CAwkInst->getFile(fileName, CFileBase::Mode::READ);
  }
  else
    return CAwkInst->getFile(stdin);
}

void
CAwkIFile::
print(std::ostream &os) const
{
  if (type_ == Type::READ_FILE)
    os << "<" << *file_;
  else
    os << "|" << *file_;
}

void
CAwkOFile::
write(const std::string &str)
{
  if      (type_ == Type::WRITE_FILE || type_ == Type::APPEND_FILE) {
    CFile *file = getFile();

    file->write(str);
  }
  else if (type_ == Type::PIPE_COMMAND) {
    std::string cmdStr = expression_->getValue()->getString();

    CAwkPipe *pipe = CAwkInst->getPipe(cmdStr, CAwkPipe::Type::OUTPUT);

    pipe->addInput(str);
  }
  else
    assert(false);
}

CFile *
CAwkOFile::
getFile() const
{
  if (expression_.isValid()) {
    std::string fileName = expression_->getValue()->getString();

    if      (type_ == Type::WRITE_FILE)
      return CAwkInst->getFile(fileName, CFileBase::Mode::APPEND);
    else if (type_ == Type::APPEND_FILE)
      return CAwkInst->getFile(fileName, CFileBase::Mode::WRITE);
    else
      assert(false);
  }
  else
    return CAwkInst->getFile(stdout);
}

void
CAwkOFile::
print(std::ostream &os) const
{
  if      (type_ == Type::WRITE_FILE)
    os << ">" << *expression_;
  else if (type_ == Type::APPEND_FILE)
    os << ">>" << *expression_;
  else if (type_ == Type::PIPE_COMMAND)
    os << "|" << *expression_;
}
