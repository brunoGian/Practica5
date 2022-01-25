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
#define TAMBUFFER 10

#define VELPROD 10000	// Microsegundos
#define VELCONS 50000

//#define HASTA 200
int DESDE, HASTA;

void productor(); // Emisor 
void consumidor(); // Receptor
int isprime(int n);

void tree_insert(struct TREE **root,int dato);
void tree_inorder(struct TREE *root);

#define PRODUCTORES 4
#define FIN -1

int queue;	// Buzón de mensajes
int status;

int main(int argc, char *argv[])
{	
	DESDE = atoi(argv[1]);
	HASTA = atoi(argv[2]);
    // Definición de variables
 	int p;
	
	/* Crear buzon de mensajes */
	queue = msgget(0x1234, 0666|IPC_CREAT);
	if(queue==-1)
	{
		fprintf(stderr,"No se pudo crear el buzón\n");
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
	if(p==0) consumidor();


  
    for(int n=0;n<PRODUCTORES;n++) wait(&status);

	msgctl(queue, IPC_RMID, NULL); /* Eliminmos el buzon */
    exit(EXIT_SUCCESS);
}


void productor(int nprod)
{	
	struct msgbuf mensaje;
    

	int inicio = nprod*HASTA/PRODUCTORES+DESDE-(DESDE/PRODUCTORES)*nprod;
	int limite = (nprod+1)*HASTA/PRODUCTORES+DESDE-(DESDE/4)*(nprod+1);
	
    printf("Inicia productor\n");
    for(int n=inicio;n<=HASTA;n++)
    {
		if(isprime(n) && n<HASTA)
		{
			
			if(n<limite)
			{
				// Construye un mensaje
				mensaje.mtype = 1; // Prioridad o tipo del mensaje
				mensaje.num = n;
				msgsnd(queue,&mensaje,sizeof(struct msgbuf),IPC_NOWAIT);	// No espera a que sea recibido
				//printf("\tProductor %d produce %d\n",nprod,mensaje.num);
			}
			else
			{
				mensaje.mtype=1;	// Prioridad o tipo del mensaje
				mensaje.num = FIN; // -1
				msgsnd(queue, &mensaje, sizeof(struct msgbuf), IPC_NOWAIT);	// No espera a que sea recibido
			}
			sleep(1);
        	//usleep(rand()%VELPROD);
		}
    }
	msgctl(queue, IPC_RMID, NULL);
    exit(0);
}


void consumidor()
{
	struct TREE *root=NULL;
    int productores=PRODUCTORES;
	int terminadores=0;
	int dato;

    printf("Inicia Consumidor\n");
	
	struct msgbuf mensaje;
    
	while(productores)
    {
		msgrcv(queue,&mensaje,sizeof(struct msgbuf),1,0);
		dato = mensaje.num;

		printf("El valor reibido es: %d\n", dato);
		//sleep(1);

		if(dato!=FIN)
		{
			// En vez de mostrarlo, lo vamos a guardar en un contenedor de manera
			// que queden ordenados en orden ascendente, por ejemplo, un árbol binario
        	// printf("\t\t\tConsumidor consume %d\n",dato);
			tree_insert(&root,dato);
		}
		else
		{
			productores--;
			terminadores++;
			printf("\t\t\tFINAL ENCONTRADO\n");
		}

        usleep(rand()%VELCONS);

		
    }
	
	// Mostrar todos los elementos que se guardaron de manera ordenada en el contenedor.
	tree_inorder(root);
	
	printf("Terminadores %d\n",terminadores);
	
    exit(0);
}
     
