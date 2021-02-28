DIR_SRC = ./src/

DIR_INC = ./include/

BDIR=$(PDW)

CFLAGS	= -Wall -W -Wextra -O3 -I$(DIR_INC) -DEPATH=\"$(BDIR)\" -L/lib/pvm3/lib

LDFLAGS = -lpvm3

RM	= rm -f

ECHO	= /bin/echo -e

SRC	= 	$(DIR_SRC)uppers.c \
	  	$(DIR_SRC)point.c

OBJ	= $(SRC:.c=.o)

CC	= gcc

MASTER=upperm
SLAVE=uppers

all: $(MASTER) $(SLAVE)

$(MASTER):$(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) -o $(MASTER)
	@$(ECHO) '\033[01;34m---------------\033[01;34m->\033[01;32mCompiled\033[01;34m<-\033[01;34m---------------\033[01;00m'

$(SLAVE):$(OBJ)
	@$(CC) $(OBJ) $(LDFLAGS) -o $(SLAVE)
	@$(ECHO) '\033[01;34m---------------\033[01;34m->\033[01;32mCompiled\033[01;34m<-\033[01;34m---------------\033[01;00m'

clean:
	@$(RM) $(OBJ)
	@$(ECHO) '\033[01;34m---------------\033[01;34m->\033[01;32m.o removed\033[01;34m<-\033[01;34m---------------\033[01;00m'

fclean:	clean
	@$(RM) $(MASTER) $(SLAVE)
	@$(ECHO) '\033[01;34m---------------\033[01;34m->\033[01;32mBinary removed\033[01;34m<-\033[01;34m---------------\033[01;00m'

re:	fclean all

.c.o:
	@$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@ && \
	 $(ECHO) "\033[01;32m" "[OK]"  $<  "\033[01;00m" || \
	 $(ECHO) "\033[0;31m" "[XX]"  $<  "\033[01;00m"

.PHONY:	re all clean fclean $(SLAVE) $(MASTER)  .c.o