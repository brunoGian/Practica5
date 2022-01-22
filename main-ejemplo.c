#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <math.h>

#define N_ITERACIONES 2000000000
#define NFORKS 4

double pi=0.0;
double *pi_result;

int leibniz(void *args){
    int tnum=*((int*) args);
    int n;
    for (n = tnum; n < N_ITERACIONES; n+=NFORKS){
        pi += ((pow(-1,n) / (2*n + 1)));
    }
    *pi_result += pi;
    printf("Hilo %d, n = %d\n", tnum, n-NFORKS);
    return 0;
}

int main(){
    struct timeval ts;

    gettimeofday(&ts, NULL);
    int t_inicial = ts.tv_sec;
    int pid_father = getpid();
    int pid;

    int shmid;
    shmid = shmget(0x1234,sizeof(double),IPC_CREAT | 0666);
    pi_result = shmat(shmid,NULL,0);
    *pi_result = 0;
    
    printf("Calculando pi...\n");

    for (int i = 0; i < NFORKS; i++){
        if (getpid() == pid_father){
            pid = fork();
        }

        if (pid == 0){
            int tnum = i;
            leibniz(&tnum);
            break;
        }
    }
    
    

    if (pid != 0){
        for (int i = 0; i < NFORKS; i++){
            wait(NULL);
        }
        
        gettimeofday(&ts, NULL);
        int t_final = ts.tv_sec;
        int dt = t_final - t_inicial;

        *pi_result *= 4;
        printf("\n========================\n\n");
        printf("Pi equivale: %1.47f\n", *pi_result);
        printf("Tiempo transcurrido: %d\n", dt);

    }
    
    
	
	shmdt(pi_result); // Desconectar la memoria compartida
    shmctl(shmid, IPC_RMID, 0);   