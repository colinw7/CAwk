.SILENT:

CC = g++

CFLAGS = -Wall -pedantic -g -I$(INC_DIR) -DPROTO
LFLAGS = -g -L$(LIB_DIR)

LIBS = \
-lCAwk -lCFile -lCEnv -lCStrParse -lCPrintF -lCStrUtil \
-lCCommand -lCReadLine -lCHistory -lCMath -lCRegExp \
-lCOSTerm -lCOSProcess -lCOSSignal -lCOSErr -lCOSTime -lCOSRand -lCOSFile -lCOSUser \
-lCGlob -lCStrNCase -lreadline -lncurses -ltre

.cpp.o:
	@echo "Compiling $<"
	$(CC) -c $(CFLAGS) $<

all: CAwkExpressionTest

CAwkExpressionTest: CAwkExpressionTest.o $(LIB_DIR)/libCAwk.a
	@echo "Linking CAwkExpressionTest"
	$(CC) -o CAwkExpressionTest CAwkExpressionTest.o $(LFLAGS) $(LIBS)

clean:
	rm -f *.o
