all: mensajes semaforos

mensajes: mensajes-Bruno.c
	gcc -o mensajes mensajes-Bruno.c -lm
semaforos: semsprodcons_proc.c
	gcc -o semaforos semsprodcons_proc.c -lm
clean: rm mensajes semaforos

