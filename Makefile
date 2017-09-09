CC = gcc
DEBUG = -O0 -g

all:
	$(CC) main.c rogmap.c -o rogmap $(DEBUG)
