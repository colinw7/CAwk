#ifndef CAWK_TYPES_H
#define CAWK_TYPES_H

#include <CRefPtr.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>

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

using CAwkActionPtr         = CRefPtr<CAwkAction>;
using CAwkActionListPtr     = CRefPtr<CAwkActionList>;
using CAwkActionBlockPtr    = CRefPtr<CAwkActionBlock>;
using CAwkExpressionPtr     = CRefPtr<CAwkExpression>;
using CAwkExpressionTermPtr = CRefPtr<CAwkExpressionTerm>;
using CAwkExprFunctionPtr   = CRefPtr<CAwkExprFunction>;
using CAwkFunctionPtr       = CRefPtr<CAwkFunction>;
using CAwkIFilePtr          = CRefPtr<CAwkIFile>;
using CAwkOperatorPtr       = CRefPtr<CAwkOperator>;
using CAwkOFilePtr          = CRefPtr<CAwkOFile>;
using CAwkPatternActionPtr  = CRefPtr<CAwkPatternAction>;
using CAwkPatternPtr        = CRefPtr<CAwkPattern>;
using CAwkValuePtr          = CRefPtr<CAwkValue>;
using CAwkVariablePtr       = CRefPtr<CAwkVariable>;
using CAwkVariableRefPtr    = CRefPtr<CAwkVariableRef>;

using CAwkActionBlockList    = std::vector<CAwkActionBlockPtr>;
using CAwkExpressionTermList = std::vector<CAwkExpressionTermPtr>;
using CAwkOperatorList       = std::vector<CAwkOperatorPtr>;
using CAwkExpressionList     = std::vector<CAwkExpressionPtr>;

#endif
