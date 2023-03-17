TAR = memory
OBJ = main.o
CC := gcc
CFLAG = -std=gnu99

$(TAR) : $(OBJ)
	$(CC) $(CFLAG) $(OBJ) -o $(TAR)

%.o : %.c
	$(CC) $(CFLAG) -c $< -o $@

.PHONY:
cleanall:
	rm -f $(OBJ) $(TAR)
clean:
	rm -f $(OBJ)