.SILENT:

#CC = g++
CC = clang++

CFLAGS = \
-Wall -pedantic -g \
-std=c++14 \
-I$(INC_DIR) \
-I$(INC_DIR)/CAwk \
-I$(INC_DIR)/CFile \
-I$(INC_DIR)/CFileType \
-I$(INC_DIR)/CUtil \
-I$(INC_DIR)/CStrParse \
-I$(INC_DIR)/CStrUtil \
-I$(INC_DIR)/CRegExp \
-DPROTO

LFLAGS = -g -L$(LIB_DIR)

LIBS = \
-lCAwk -lCFile -lCEnv -lCStrParse -lCPrintF -lCStrUtil \
-lCCommand -lCReadLine -lCHistory -lCMath -lCRegExp \
-lCOS -lCGlob -lreadline -lncurses -ltre

.cpp.o:
	@echo "Compiling $<"
	$(CC) -c $(CFLAGS) $<

all: CAwkExpressionTest

CAwkExpressionTest: CAwkExpressionTest.o $(LIB_DIR)/libCAwk.a
	@echo "Linking CAwkExpressionTest"
	$(CC) -o CAwkExpressionTest CAwkExpressionTest.o $(LFLAGS) $(LIBS)

clean:
	rm -f CAwkExpressionTest *.o
