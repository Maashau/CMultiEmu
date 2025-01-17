CC=gcc -Wall -Wextra
OBJ=obj
LIB=lib
ASMDIR=prg
APPLE_I_LIB=apple_i/lib
C64_LIB=c64/lib

all : $(LIB) $(OBJ) test_65xx.out assemble

assemble :
	make -C $(ASMDIR) -B

test_65xx.out : $(OBJ)/main.o $(LIB)/65xx.lib $(APPLE_I_LIB)/apple_i.lib $(C64_LIB)/c64.lib
	$(CC) -o $@ $^ -lpthread

$(APPLE_I_LIB)/apple_i.lib :
	make -C apple_i -B

$(C64_LIB)/c64.lib :
	make -C c64 -B

$(LIB)/65xx.lib : $(OBJ)/65xx.o $(OBJ)/65xx_opc.o $(OBJ)/65xx_addrm.o #$(OBJ)/65xx_assembler.o
	ar rcs $@ $^

$(OBJ)/%.o : $(OBJ)
	$(CC) -g -c $*.c -o $@

$(OBJ) :
	mkdir -p $@

$(LIB) :
	mkdir -p $@
