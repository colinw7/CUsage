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
CRegExp.cpp \
CDirFTW.cpp

CUsage.cpp: CUsage.h CFileUtil.h

CUsage.h: CRegExp.h CDirFTW.h CStrUtil.h CFuncs.h

CRegExp.cpp: CRegExp.h CStrUtil.h

CDirFTW.cpp: CDirFTW.h

CDirFTW.h: CFileType.h

CFileUtil.h: CFileType.h

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = -I$(INC_DIR) -I.

.SUFFIXES: .cpp

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CUsage: $(OBJ_DIR)/CUsage.o $(OBJ_DIR)/CRegExp.o $(OBJ_DIR)/CDirFTW.o
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CUsage $(OBJS) -L$(LIB_DIR) -lCStrUtil -ltre
