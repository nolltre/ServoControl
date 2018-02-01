all: servo

servo: servo.c
	gcc -o servo servo.c -pthread -lpigpio -lrt -lncurses
