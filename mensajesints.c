/*
 * En esta versión el receive es bloquente 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <math.h>

#define TAMBUFFER 10

#define VELPROD 10000  // Microsegundos
#define VELCONS 50000

// NUEVAS VARIABLES
int DESDE, HASTA;

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    unsigned int num;   /* message data */
	int nprod;
};

struct STRBUFF {
	int ent;	// Donde va a estar el siguiente elemento que voy a meter al buffer
	int sal;	// Donde está el siguiente elemento que voy a sacar del buffer
	int buffer[TAMBUFFER]; // Buffer circular
	};

struct STRBUFF *bf;

// NUEVA ESTRUCTURA DE ARBOL
struct TREE {
	int dato;
	struct TREE *left;
	struct TREE *right;
};

int queue;	// Buzón de mensajes

void emisor();
void receptor();
int isprime(int n);

//#define ITERS 10
//#define FIN ITERS-1

#define PRODUCTORES 4
#define FIN -1

int main(int argc, char *argv[])
{
	//printf("Entradas: %s %s\n", argv[1], argv[2]);
	DESDE = atoi(argv[1]);
	HASTA = atoi(argv[2]);
    // Definición de variables
    int res;
    int n;
 	int p;
	int i;
	int shmid;

	int status;

	// Crear buzón de mensajes 
	queue=msgget(0x1234,0666|IPC_CREAT);
	if(queue==-1)
	{
		fprintf(stderr,"No se pudo crear el buzón\n");
		exit(1);
	}
	
	// Crea un proceso donde va a ejecutarse el emisor
	for(i=0;i<PRODUCTORES;i++)
	{
		p=fork();
		if(p==0)
			emisor(i);
	}
	
	// Crea un proceso donde va a ejecutarse el receptor
	p=fork();
	if(p==0)
		receptor();	// El hijo ejecuta el receptor
	
	// Esperar a que los dos procesos terminen
	wait(&status); //////???????????????
	wait(&status); //////???????????????

	for(n=0;n<2;n++)
  		wait(NULL);
	
	msgctl(queue, IPC_RMID, NULL);

	shmdt(bf);	// Desconectar la memoria compartida al terminar
	shmctl(shmid,IPC_RMID,NULL);	// Pedir al SO que elimine la memoria compartida

	exit(EXIT_SUCCESS);
}

void emisor(int nprod)
{
	int n;

	int inicio = nprod*HASTA/PRODUCTORES+DESDE-(DESDE/PRODUCTORES)*nprod;
	int limite = (nprod+1)*HASTA/PRODUCTORES+DESDE-(DESDE/4)*(nprod+1);

	struct msgbuf mensaje;

	for(n=inicio;n<=limite;n++)
	{
		if(isprime(n) || n==limite){
		// Construye un mensaje
		mensaje.mtype=1;	// Prioridad o tipo del mensaje
		//mensaje.num=n;

		if(n<limite)
			{
        		bf->buffer[bf->ent]=n;
				printf("Productor %d produce %d\n",nprod,n);
				mensaje.num=n;
			}
			else
			{
				bf->buffer[bf->ent]=FIN;
				// printf("\t\t\tLIMITE ENCONTRADO\n");
			}
				
			
			bf->ent++;
			if(bf->ent==TAMBUFFER)	// Si TAMBUFFER es 10, 0 1 2 3 4 5 6 7 8 9
				bf->ent=0;		// 0 = Terminador
			
			usleep(rand()%VELPROD);


		// Envíe el mensaje al buzón
		msgsnd(queue,&mensaje,sizeof(struct msgbuf),IPC_NOWAIT);	// No espera a que sea recibido
		//sleep(1);
		usleep(rand()%VELPROD);
		}
	}
	
	exit(0);	// Termina el emisor
}


void receptor()
{
	struct msgbuf mensaje;
	do 
	{
		// Recibe un mensaje del buzón
		msgrcv(queue,&mensaje,sizeof(struct msgbuf),1,0);
		printf("Productor %d produce %d\n",mensaje.nprod,mensaje.num);
		//printf("%d\n",mensaje.num);	// Imprimir el mensaje
	} while(mensaje.num!=FIN); // Mientras no sea fin 
	exit(0);	// Termina el receptor
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

