#include <CAwk.h>
#include <CFile.h>
#include <cstring>

int
main(int argc, char **argv)
{
  std::vector<std::string > args;
  std::string               progFile;
  std::string               progText;

  bool debug = false;

  args.push_back(argv[0]);

  for (int i = 1; i <argc; ++i) {
    if (argv[i][0] == '-') {
      if      (strcmp(&argv[i][1], "f") == 0)
        progFile = argv[++i];
      else if (strcmp(&argv[i][1], "-debug") == 0)
        debug = true;
      else
        std::cerr << "Invalid option '" << argv[i] << "'" << std::endl;
    }
    else {
      if (progFile == "" && progText == "")
        progText = argv[i];
      else
        args.push_back(argv[i]);
    }
  }

  CAwkInst->init(args);

  if (debug)
    CAwkInst->setDebug();

  if      (progFile != "") {
    if (! CAwkInst->parseFile(progFile))
      exit(1);
  }
  else if (progText != "") {
    if (! CAwkInst->parseLine(progText))
      exit(1);
  }
  else {
    std::cerr << "Usage: CAwk [-f <file>] [<str>]" << std::endl;
    exit(1);
  }

  CAwkInst->process();

  return 0;
}
