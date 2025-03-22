#ifndef CAWK_TYPES_H
#define CAWK_TYPES_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <iostream>
#include <cassert>

using StringVectorT = std::vector<std::string>;

class CAwk;
class CAwkAction;
class CAwkActionList;
class CAwkActionBlock;
class CAwkExpression;
class CAwkExpressionTerm;
class CAwkExprFunction;
class CAwkFunction;
class CAwkIFile;
class CAwkOperator;
class CAwkOFile;
class CAwkPattern;
class CAwkPatternAction;
class CAwkValue;
class CAwkVariable;
class CAwkVariableRef;

using CAwkActionPtr         = std::shared_ptr<CAwkAction>;
using CAwkActionListPtr     = std::shared_ptr<CAwkActionList>;
using CAwkActionBlockPtr    = std::shared_ptr<CAwkActionBlock>;
using CAwkExpressionPtr     = std::shared_ptr<CAwkExpression>;
using CAwkExpressionTermPtr = std::shared_ptr<CAwkExpressionTerm>;
using CAwkExprFunctionPtr   = std::shared_ptr<CAwkExprFunction>;
using CAwkFunctionPtr       = std::shared_ptr<CAwkFunction>;
using CAwkIFilePtr          = std::shared_ptr<CAwkIFile>;
using CAwkOperatorPtr       = std::shared_ptr<CAwkOperator>;
using CAwkOFilePtr          = std::shared_ptr<CAwkOFile>;
using CAwkPatternActionPtr  = std::shared_ptr<CAwkPatternAction>;
using CAwkPatternPtr        = std::shared_ptr<CAwkPattern>;
using CAwkValuePtr          = std::shared_ptr<CAwkValue>;
using CAwkVariablePtr       = std::shared_ptr<CAwkVariable>;
using CAwkVariableRefPtr    = std::shared_ptr<CAwkVariableRef>;

using CAwkActionBlockList    = std::vector<CAwkActionBlockPtr>;
using CAwkExpressionTermList = std::vector<CAwkExpressionTermPtr>;
using CAwkOperatorList       = std::vector<CAwkOperatorPtr>;
using CAwkExpressionList     = std::vector<CAwkExpressionPtr>;

#endif
