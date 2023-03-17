TAR = mainmachine
OBJ = $(subst .c,.o,$(wildcard *.c))
CC := gcc
CFLAG = -std=gnu99 -Wall

$(TAR) : $(OBJ)
	$(CC) $(CFLAG) $(OBJ) -o $(TAR)
	rm -f $(OBJ)

%.o : %.c
	$(CC) $(CFLAG) -c $< -o $@

.PHONY:
cleanall:
	rm -f $(OBJ) $(TAR)
clean:
	rm -f $(OBJ)