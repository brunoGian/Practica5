#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <math.h>

/* ESTO ES PARTE DE LOS SEMAFORS
#include "semaphoresarr.h"
//*/

#define TAMBUFFER 10

#define VELPROD 10000	// Microsegundos
#define VELCONS 50000

//#define HASTA 200
int DESDE, HASTA;

void productor(); // Emisor 
void consumidor(); // Receptor
int isprime(int n);

struct STRBUFF {
	int ent;	// Donde va a estar el siguiente elemento que voy a meter al buffer
	int sal;	// Donde está el siguiente elemento que voy a sacar del buffer
	int buffer[TAMBUFFER];	// Buffer circular
};

struct TREE {
	int dato;
	struct TREE *left;
	struct TREE *right;
};

struct STRBUFF *bf;

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    unsigned int num;   /* message data */
};

void tree_insert(struct TREE **root,int dato);
void tree_inorder(struct TREE *root);

/* ESTO ES PARTE DE LOS SEMAFORS
SEM_ID semarr;
enum {E_MAX,N_BLOK,S_EXMUT};  // Semáforos 0,1 y 2
//*/

#define PRODUCTORES 4
#define FIN -1

int queue;	// Buzón de mensajes

int main(int argc, char *argv[])
{	
	DESDE = atoi(argv[1]);
	HASTA = atoi(argv[2]);
    // Definición de variables
    int res;
    int n;
 	int p;
	int i;
	int shmid;
	
	/* Crear buzon de mensajes */
	queue=msgget(0x1234,0666|IPC_CREAT);
	if(queue==-1)
	{
		fprintf(stderr,"No se pudo crear el buzón\n");
		exit(1);
	}

	srand(getpid());
	/* ESTO ES PARTE DE LOS SEMAFORS
    // Creación del arreglo de semáforos
    semarr=createsemarray((key_t) 0x1234,3);
	

    initsem(semarr,E_MAX,TAMBUFFER);
    initsem(semarr,N_BLOK,0);
    initsem(semarr,S_EXMUT,1);

    printf("Semáforos creados\n");
	//*/

	// Crear la memoria compartida
	shmid=shmget((key_t) 0x1235,sizeof(struct STRBUFF),0666|IPC_CREAT);
	bf=shmat(shmid,NULL,0);
	
    /* Aquí se crean los procesos */
	for(i=0;i<PRODUCTORES;i++)
	{
		p=fork();
		if(p==0)
			productor(i);
	}

	p=fork();
	if(p==0) consumidor();


  
    for(n=0;n<4;n++) wait(NULL);

	/* ESTO ES PARTE DE LOS SEMAFORS
    // Borrar los semáforos
    erasesem(semarr,E_MAX);
	erasesem(semarr,N_BLOK);
	erasesem(semarr,S_EXMUT);
	//*/
	
	shmdt(bf);	// Desconectar la memoria compartida al terminar
	shmctl(shmid,IPC_RMID,NULL);	// Pedir al SO que elimine la memoria compartida

	msgctl(queue, IPC_RMID, NULL);

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
		if(isprime(n) || n==HASTA)
		{
			/* ESTO ES PARTE DE LOS SEMAFORS
			semwait(semarr,E_MAX);	// Si se llena el buffer se bloquea
        	semwait(semarr,S_EXMUT);	// Asegurar el buffer como sección crítica
			//*/
			if(n<limite)
			{
        		//bf->buffer[bf->ent]=n;
				// Construye un mensaje
				mensaje.mtype=1;	// Prioridad o tipo del mensaje
				mensaje.num = n;
				msgsnd(queue,&mensaje,sizeof(struct msgbuf),IPC_NOWAIT);	// No espera a que sea recibido
				printf("Productor %d produce %d\n",nprod,n);
			}
			else
			{
				mensaje.mtype=1;	// Prioridad o tipo del mensaje
				mensaje.num = FIN; // -1
				msgsnd(queue,&mensaje,sizeof(struct msgbuf),IPC_NOWAIT);	// No espera a que sea recibido
				//bf->buffer[bf->ent]=FIN;
				// printf("\t\t\tHASTA ENCONTRADO\n");
			}
				
			/*
			bf->ent++;
			if(bf->ent==TAMBUFFER)	// Si TAMBUFFER es 10, 0 1 2 3 4 5 6 7 8 9
				bf->ent=0;		// 0 = Terminador
			//*/
 
        	usleep(rand()%VELPROD);

			/* ESTO ES PARTE DE LOS SEMAFORS
        	semsignal(semarr,S_EXMUT);	// Libera la sección crítica del buffer
        	semsignal(semarr,N_BLOK);	// Si el consumidor está bloqueado porque el buffer está vacío, lo desbloqueas
			

        	usleep(rand()%VELPROD);
			//*/
		}
    }
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
		
		/* ESTO ES PARTE DE LOS SEMAFORS
        semwait(semarr,N_BLOK);	// Si el buffer está vacío, se bloquea
        semwait(semarr,S_EXMUT);	// Asegura el buffer como sección crítica 
		//*/


		//dato=bf->buffer[bf->sal];
		msgrcv(queue,&mensaje,sizeof(struct msgbuf),1,0);
		dato = mensaje.num;

		printf("%d %d\n", dato, productores);
		sleep(1);

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

		/*
		bf->sal++;
		if(bf->sal==TAMBUFFER)
			bf->sal=0;
		//*/
        //usleep(rand()%VELCONS);

		/* ESTO ES PARTE DE LOS SEMAFORS
        semsignal(semarr,S_EXMUT);	// Libera la SC el buffer
        semsignal(semarr,E_MAX);	// Si el productor está bloqueado porque el buffer estaba lleno, lo desbloquea
		
        usleep(rand()%VELCONS);
		//*/
		
    }
	
	// Mostrar todos los elementos que se guardaron de manera ordenada en el contenedor.
	tree_inorder(root);
	
	printf("Terminadores %d\n",terminadores);
	
    exit(0);
}
     
int isprime(int n)
{
	int d=3;
	int prime=0;
	int limit=sqrt(n);
	
	if(n<2)
		prime=0;
	else if(n==2)
		prime=1;
	else if(n%2==0)
		prime=0;
	else
	{
		while(d<=limit && n%d)
			d+=2;
		prime=d>limit;
	}
	return(prime);
}


void tree_insert(struct TREE **root,int dato)
{
	if(*root==NULL)
	{
		*root=malloc(sizeof(struct TREE));
		(*root)->dato=dato;
		(*root)->left=NULL;
		(*root)->right=NULL;
	}
	else if(dato < (*root)->dato)
		tree_insert(&(*root)->left,dato);
	else if(dato > (*root)->dato)
		tree_insert(&(*root)->right,dato);
	
}

void tree_inorder(struct TREE *root)
{
	if(root->left!=NULL)
		tree_inorder(root->left);
	printf("%d\n",root->dato);
	if(root->right!=NULL)
		tree_inorder(root->right);
	
}
