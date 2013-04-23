CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR  = ../include
OBJ_DIR  = ../obj
EXEC_DIR = ../exec

all: $(EXEC_DIR)/CUsage

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(EXEC_DIR)/CUsage

CUsage.cpp: CUsage.h CFileUtil.h

CUsage.h: CRegExp.h CDirFTW.h CStrUtil.h CFuncs.h

CRegExp.cpp: CRegExp.h CStrUtil.h

CDirFTW.cpp: CDirFTW.h

CDirFTW.h: CFileType.h

CFileUtil.h: CFileType.h

.SUFFIXES: .cpp

$(OBJ_DIR)/CUsage.o: CUsage.cpp
	$(CC) $(CDEBUG) -o $(OBJ_DIR)/CUsage.o -c CUsage.cpp -I$(INC_DIR) -I.

$(OBJ_DIR)/CRegExp.o: CRegExp.cpp
	$(CC) $(CDEBUG) -o $(OBJ_DIR)/CRegExp.o -c CRegExp.cpp -I$(INC_DIR) -I.

$(OBJ_DIR)/CDirFTW.o: CDirFTW.cpp
	$(CC) $(CDEBUG) -o $(OBJ_DIR)/CDirFTW.o -c CDirFTW.cpp -I$(INC_DIR) -I.

$(EXEC_DIR)/CUsage: $(OBJ_DIR)/CUsage.o $(OBJ_DIR)/CRegExp.o $(OBJ_DIR)/CDirFTW.o
	$(CC) $(LDEBUG) -o $(EXEC_DIR)/CUsage $(OBJ_DIR)/CUsage.o $(OBJ_DIR)/CRegExp.o $(OBJ_DIR)/CDirFTW.o -ltre
