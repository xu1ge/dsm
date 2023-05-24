TAR = stream
OBJ = $(subst .c,.o,$(wildcard *.c))
OBJ_ALL = $(wildcard *.o)
CC := gcc
CFLAG = -std=gnu99 -Wall -lpthread
DIR_IND := ../stream

$(TAR) : $(OBJ)
	@echo "make rpc"
	cd rpc/temp/ && $(MAKE)
	cd ../../
	@echo "make main"
	cd main/ && $(MAKE)
	cd ../
	$(CC) $(CFLAG) $(OBJ_ALL) -o $(TAR)
	rm -f $(OBJ_ALL)

%.o : %.c
	$(CC) $(CFLAG) -c $< -o $@ -I $(DIR_IND)

.PHONY:
cleanall:
	rm -f $(OBJ_ALL) $(TAR)
clean:
	rm -f $(OBJ_ALL)