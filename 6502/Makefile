CC=gcc
OBJ=obj
LIB=lib

all : $(LIB) $(OBJ) test_6502.out

test_6502.out : $(OBJ)/main.o $(LIB)/6502.lib
	$(CC) -o $@ $^

$(LIB)/6502.lib : $(OBJ)/6502.o $(OBJ)/6502_opc.o $(OBJ)/6502_addrm.o $(OBJ)/6502_assembler.o
	ar rcs $@ $^

$(OBJ)/%.o : $(OBJ)
	$(CC) -g -c $*.c -o $@

$(OBJ) :
	mkdir -p $@

$(LIB) :
	mkdir -p $@