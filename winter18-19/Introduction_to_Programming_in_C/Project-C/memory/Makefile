CC=gcc
GTKFLAGS = $(shell pkg-config gtk+-3.0 --cflags --libs)
CFLAGS = -std=c11 -Wextra -Werror -Wall -Wno-unused-parameter
NAME=final

SRC=plansza.c lin-fifo.c okna.c gra.c main.c
OBJ = plansza.o lin-fifo.o okna.o gra.o main.o

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(GTKFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(GTKFLAGS)

clean:
		rm -f $(OBJ) 
