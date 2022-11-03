CC=gcc
FLAGS=-Wall -g
LINKS=-lpthread

all: tetris.c render.c logic.c quit_utils.c shadow.c
	$(CC) $(FLAGS) -c render.c $(LINKS)
	$(CC) $(FLAGS) -c tetris.c $(LINKS)
	$(CC) $(FLAGS) -c logic.c $(LINKS)
	$(CC) $(FLAGS) -c quit_utils.c $(LINKS)
	$(CC) $(FLAGS) -c input.c $(LINKS)
	$(CC) $(FLAGS) -c rotate.c $(LINKS)
	$(CC) $(FLAGS) -c shadow.c $(LINKS)
	$(CC) $(FLAGS) tetris.o render.o logic.o -o tetris quit_utils.o input.o rotate.o shadow.o $(LINKS)
