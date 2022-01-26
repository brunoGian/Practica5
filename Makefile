all: mensajes semaforos

mensajes: pasoDeMensajes.c 
	gcc -o mensajes pasoDeMensajes.c -lm
semaforos: semsprodcons_proc.c
	gcc -o semaforos semsprodcons_proc.c -lm
clean: rm mensajes semaforos

