CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = .
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CAwk

SRC = \
CAwkTest.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-I. \
-I$(INC_DIR) \
-I../../CStrUtil/include \
-I../../CRegExp/include \
-I../../CFile/include \
-I../../CUtil/include \

LFLAGS = \
-L$(LIB_DIR) \
-L../../CAwk/lib \
-L../../CCommand/lib \
-L../../CReadLine/lib \
-L../../CFile/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../COS/lib \

LIBS = \
-lCAwk -lCCommand -lCReadLine -lCFile -lCStrUtil -lCRegExp -lCOS \
-lreadline -ltre -lncurses

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CAwk

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CAwk: $(OBJS) $(LIB_DIR)/libCAwk.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CAwk $(OBJS) $(LFLAGS) $(LIBS)
