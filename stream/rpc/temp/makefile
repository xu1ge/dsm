TAR = rpc
OBJ = $(subst .c,.o,$(wildcard *.c))
CC := gcc
CFLAG = -std=gnu99 -Wall -lpthread
DIR_IND := ../temp

$(TAR) : $(OBJ)
	# $(CC) $(CFLAG) $(OBJ) -o $(TAR)
	mv $(OBJ) ../../

%.o : %.c
	$(CC) $(CFLAG) -c $< -o $@ -I $(DIR_IND)

.PHONY:
cleanall:
	rm -f $(OBJ) $(TAR)
clean:
	rm -f $(OBJ)