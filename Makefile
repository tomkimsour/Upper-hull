CFLAGS = -g -Wall -pedantic -O3
LDFLAGS = 
#UNIT = -ftest-coverage -fprofile-arcs

SRC = $(wildcard src/*.c)
OBJ = $(addprefix obj/, $(addsuffix .o, $(basename $(notdir $(SRC)))))

INCLUDE = $(wildcard include/*.h)
NAME = upper_hull 

all: $(NAME)

remake: clean $(NAME)

$(NAME): $(OBJ)
	gcc obj/* $(LDFLAGS) -o -lpvm3 $@ 

obj/%.o: src/%.c $(INCLUDE)
	gcc -c -Iinclude $(CFLAGS) -o $@ $<

clean:
	rm obj/*.o test/obj/*.o *.out *.gch -f

clear:
	rm obj/*.o *.gch -f
