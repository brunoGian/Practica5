I OBJETIVOS  
En esta práctica el alumno: 
 
• Utilizará los semáforos como una herramienta para la sincronización de procesos 
concurrentes. 
• Conocerá y usará algunos mecanismos de comunicación entre procesos que provee el 
sistema operativo tales como los buzones de mensajes. 
 
 
 
II BIBLIOGRAFÍA  
• William Stallings, “SISTEMAS OPERATIVOS”,Prentice Hall. 
• Silberschatz, Galvin, Gagne, “SISTEMAS OPERATIVOS”, Mc Graw Hill. 
• Neil Matthew & Richard Stones, “BEGINNING LINUX PROGRAMMING”, Wrox. 
 
 
III  RECURSOS  
• Una estación de trabajo con Linux con su ambiente gráfico. 
• Un editor de texto. 
• El compilador de C GNU. 
 
 
IV  ACTIVIDADES  
1 Ejercicio a realizar 
 
Para esta práctica vamos a hacer una versión paralela del programa que obtiene la lista 
de números primos dentro de un rango introducido por el usuario en la línea de comandos. 
Por ejemplo: 
 
$ ./primos 1 20

2 
3 
5 
7 
11 
13 
17 
19 
 
En esta ocasión lo vamos a resolver con procesos donde vamos a tener 4 procesos que buscan 
números primos a los que vamos a llamar buscadores, y un proceso que va a recibir los números 
primos encontrados para desplegarlos en pantalla al que vamos a llamar mostrador.  
 
Los procesos buscadores, van a buscar dentro de un conjunto de números dentro del rango 
introducido por el usuario cuáles son primos, y cada que encuentra uno se lo enviará al proceso 
mostrador para que lo almacene en una lista encadenada y lo despliegue en pantalla. 
 
Ya que la ejecución de los procesos buscadores será en paralelo donde cada uno de ellos buscará 
números primos en un conjunto de números diferente y conforme los vayan encontrando los irán 
enviando al proceso mostrador. El proceso mostrador al recibir cada número lo almacenará en 
una lista encadenada ordenada para que cuando se reciban todos los números se muestren en 
pantalla en orden ascendente.  
 
1.1 Solución con semáforos. 
 
Utilizando un espacio de memoria compartida entre procesos y los semáforos que provee el 
sistema operativo Linux (Tipo UNIX) implementa el mecanismo mediante el cual los procesos 
buscadores enviarán los números primos encontrados al proceso mostrador.  
 
Las llamadas que provee el sistema operativo para obtener un segmento en memoria compartida 
son: shmget, shmat, shmctl. Las llamadas para pedir y usar semáforos son de bajo nivel: 
semget, semop, semctl, por lo que es necesario que tengas en un archivo llamado 
semaphores.h donde estén definidas las funciones que faciliten el uso de los semáforos 
 
Un ejemplo de este archivo lo puedes encontrar en el Dropbox del curso. Es necesario que en la 
terminación de ejecución del programa se eliminen todos los semáforos que hayan sido creados. 
 
Importante: Esta solución se tiene que hacer con procesos y emplear los mecanismos de 
comunicación y sincronización de procesos que aquí mismo se indican, no se aceptarán 
soluciones que utilicen hilos.  

2. Solución con buzones de mensajes 
 
Utilizando las colas de mensajes o buzones que provee el sistema operativo Linux (Tipo UNIX) 
implementa el mecanismo mediante el cual los procesos buscadores enviarán los números 
primos encontrados al proceso mostrador.  
 
Las llamadas que provee el sistema operativo para pedir y usar colas de mensajes son: msgget, 
msgsnd, msgrcv. Es necesario que en la terminación de ejecución del programa se eliminen 
todas las colas de mensajes que hayan sido creadas. 
Importante: Esta solución se tiene que hacer con procesos, no se aceptarán soluciones que 
utilicen hilos. 
 
Importante: Esta solución se tiene que hacer con procesos y emplear los mecanismos de 
comunicación y sincronización de procesos que aquí mismo se indican, no se aceptarán 
soluciones que utilicen hilos. 
