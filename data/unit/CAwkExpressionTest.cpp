#include <CAwk/CAwk.h>

void testExpression(const std::string &expr);

int
main(int argc, char **argv)
{
  for (int i = 1; i < argc; ++i)
    testExpression(argv[i]);

  return 0;
}

void
testExpression(const std::string &expr)
{
  StringVectorT args;

  CAwkInst->init(args);

  CAwkInst->parseInit(expr);

  CAwkExpressionPtr expression;

  if (CAwkInst->parseExpression(&expression)) {
    std::cout << *expression << std::endl;

    CAwkValuePtr value = expression->execute()->getValue();

    if (value.isValid())
      std::cout << *value << std::endl;
  }
}
