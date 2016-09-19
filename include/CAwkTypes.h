#ifndef CAWK_TYPES_H
#define CAWK_TYPES_H

#include <CRefPtr.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>

typedef std::vector<std::string> StringVectorT;

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

typedef CRefPtr<CAwkAction>         CAwkActionPtr;
typedef CRefPtr<CAwkActionList>     CAwkActionListPtr;
typedef CRefPtr<CAwkActionBlock>    CAwkActionBlockPtr;
typedef CRefPtr<CAwkExpression>     CAwkExpressionPtr;
typedef CRefPtr<CAwkExpressionTerm> CAwkExpressionTermPtr;
typedef CRefPtr<CAwkExprFunction>   CAwkExprFunctionPtr;
typedef CRefPtr<CAwkFunction>       CAwkFunctionPtr;
typedef CRefPtr<CAwkIFile>          CAwkIFilePtr;
typedef CRefPtr<CAwkOperator>       CAwkOperatorPtr;
typedef CRefPtr<CAwkOFile>          CAwkOFilePtr;
typedef CRefPtr<CAwkPatternAction>  CAwkPatternActionPtr;
typedef CRefPtr<CAwkPattern>        CAwkPatternPtr;
typedef CRefPtr<CAwkValue>          CAwkValuePtr;
typedef CRefPtr<CAwkVariable>       CAwkVariablePtr;
typedef CRefPtr<CAwkVariableRef>    CAwkVariableRefPtr;

typedef std::vector<CAwkActionBlockPtr>    CAwkActionBlockList;
typedef std::vector<CAwkExpressionTermPtr> CAwkExpressionTermList;
typedef std::vector<CAwkOperatorPtr>       CAwkOperatorList;
typedef std::vector<CAwkExpressionPtr>     CAwkExpressionList;

#endif
