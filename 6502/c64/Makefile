CC=gcc -Wall -Wextra -I../
OBJ=obj
LIB=lib
ASMDIR=./roms

OBJFILES=\
$(OBJ)/c64.o\
$(OBJ)/c64_peripherals.o\
$(OBJ)/c64_vic.o\
$(OBJ)/c64_cia.o

all : $(LIB) $(OBJ) $(LIB)/c64.lib assemble

assemble :
#	make -C $(ASMDIR) -B

$(LIB)/c64.lib : $(OBJFILES)
	ar rcs $@ $^

$(OBJ)/%.o : $(OBJ)
	$(CC) -g -c $*.c -o $@

$(OBJ) :
	mkdir -p $@

$(LIB) :
	mkdir -p $@
