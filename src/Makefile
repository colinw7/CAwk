CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: $(LIB_DIR)/libCAwk.a

SRC = \
CAwkAction.cpp \
CAwk.cpp \
CAwkExecuteStack.cpp \
CAwkExpression.cpp \
CAwkFunction.cpp \
CAwkOperator.cpp \
CAwkPattern.cpp \
CAwkValue.cpp \
CAwkVariable.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CFLAGS = \
-std=c++11 \
-I$(INC_DIR) \
-I. \
-I../../CCommand/include \
-I../../CReadLine/include \
-I../../CFileUtil/include \
-I../../CFile/include \
-I../../CMath/include \
-I../../CRegExp/include \
-I../../CStrUtil/include \
-I../../COS/include \
-I../../CUtil/include \

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CFLAGS)

$(LIB_DIR)/libCAwk.a: $(OBJS)
	$(AR) crv $(LIB_DIR)/libCAwk.a $(OBJS)

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCAwk.a
