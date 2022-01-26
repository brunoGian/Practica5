#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <math.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "herramientas-mensajes.h"

#define VELPROD 50000	// Microsegundos
#define VELCONS 10000

#define PRODUCTORES 4
#define FIN -1

void productor();
void consumidor();


int DESDE, HASTA;
int queue;	// Buzón de mensajes

int main(int argc, char *argv[])
{
    int p;
	
    DESDE = atoi(argv[1]);
	HASTA = atoi(argv[2]);

    //	Crear Buzón
	 queue = msgget(0x1234, 0666|IPC_CREAT);
     if(queue == -1)
     {
		fprintf(stderr, "No se pudo crear el buzón :(\n");
		exit(1);
     }

    /* Aquí se crean los procesos */
	for(int i=0;i<PRODUCTORES;i++)
	{
		p=fork();
		if(p==0)
			productor(i);
	}

	p=fork();
	if(p==0)
		consumidor();

    for(int n=0;n<PRODUCTORES;n++)
  		wait(NULL);

    //ELIMINAMOS EL BUZON
    msgctl(queue, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}


void productor(int nprod)
{
    printf("Inicia productor\n");
    
    struct msgbuf mensaje;

    int inicio = nprod*HASTA/PRODUCTORES+DESDE-(DESDE/PRODUCTORES)*nprod;
	int limite = (nprod+1)*HASTA/PRODUCTORES+DESDE-(DESDE/4)*(nprod+1);
    //printf("nprod vale %d inicio vale %d lim1 vale %d lim vale %d\n", nprod, inicio, limite1,limite);

    for(int n=inicio;n<=limite;n++)
    {
		if(isprime(n) || n==limite) // El codigo falla si usamos "if(isprime(n) || n<=limite)"

		{
			if(n<limite)
			{

        		// Enviar n al consumidor
        		// Construye un mensaje
				mensaje.mtype = 1;	// Prioridad o tipo del mensaje
				mensaje.num = n;
				printf("Proc %d produce %d\n",nprod,mensaje.num);

                // Envíe el mensaje al buzón

				msgsnd(queue, &mensaje, sizeof(struct msgbuf), IPC_NOWAIT);	// No espera a que sea recibido

                //printf("men%d\n",mensaje.num);
                //sleep(1);
			}

			else
			{
				// Enviar 0 al consumidor

        		// Construye un mensaje
				mensaje.mtype = 1;	// Prioridad o tipo del mensaje
				mensaje.num = FIN;

				// Envíe el mensaje al buzón
				msgsnd(queue, &mensaje, sizeof(struct msgbuf), IPC_NOWAIT);	// No espera a que sea recibido
                printf("\tconsumidor: %d\n",mensaje.num );

			}
        	usleep(rand()%VELPROD);
        	usleep(rand()%VELPROD);
		}
    }
    exit(0);
}


void consumidor()

{
   struct TREE *root=NULL;
   int productores=PRODUCTORES, dato, terminadores = 0;
   struct msgbuf mensaje;

   printf("Inicia Consumidor\n");

    while(productores)
    {
        msgrcv(queue, &mensaje, sizeof(struct msgbuf), 1, 0);
		printf("Recibido: %d\n",mensaje.num);
        dato=mensaje.num;

        if(dato != FIN)
		{
			tree_insert(&root,dato);
		}
		else
		{
			productores--;	
			terminadores++;
			printf("\t\t\tFINAL ENCONTRADO\n");
		}
        usleep(rand()%VELCONS);
        usleep(rand()%VELCONS);
    }

	// Mostrar todos los elementos que se guardaron de manera ordenada en el contenedor.

	tree_inorder(root);
	printf("Terminadores %d\n",terminadores);
	
    msgctl(queue, IPC_RMID, NULL);
    exit(0);
}