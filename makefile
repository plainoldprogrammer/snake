all: snake

snake: snake.o
	gcc -o snake snake.o -lallegro -lallegro_main

snake.o: main.c
	gcc -c main.c -o snake.o

clean:
	rm snake.o snake

