CC=gcc -Wall -Wextra -I../
OBJ=obj
LIB=lib
ASMDIR=./roms

all : $(LIB) $(OBJ) $(LIB)/apple_i.lib assemble

assemble :
	make -C $(ASMDIR) -B

$(LIB)/apple_i.lib : $(OBJ)/apple_i.o
	ar rcs $@ $^

$(OBJ)/%.o : $(OBJ)
	$(CC) -g -c $*.c -o $@

$(OBJ) :
	mkdir -p $@

$(LIB) :
	mkdir -p $@
