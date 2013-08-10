CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CUsage

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CUsage

SRC = \
CUsage.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-I$(INC_DIR) \
-I../../CFile/include \
-I../../CRegExp/include \
-I../../CStrUtil/include \
-I../../CUtil/include \
-I.

LFLAGS = \
$(LEBUG) \
-L$(LIB_DIR) \
-L../../CFile/lib \
-L../../CRegExp/lib \
-L../../CStrUtil/lib \
-lCFile \
-lCRegExp \
-lCStrUtil

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CUsage: $(OBJS)
	$(CC) -o $(BIN_DIR)/CUsage $(OBJS) $(LFLAGS) -ltre
