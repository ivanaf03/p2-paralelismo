//Iván Álvarez Fernández
//Marcos Gómez Tejón

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void inicializaCadena(char *cadena, int n){
    int i;
    for(i=0; i<n/2; i++){
        cadena[i] = 'A';
    }
    for(i=n/2; i<3*n/4; i++){
        cadena[i] = 'C';
    }
    for(i=3*n/4; i<9*n/10; i++){
        cadena[i] = 'G';
    }
    for(i=9*n/10; i<n; i++){
        cadena[i] = 'T';
    }
    cadena[n] = '\0';
}

int main(int argc, char *argv[])
{
    int numprocs, rank, sum, ret_value;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc != 3){
        printf("Numero incorrecto de parametros\nLa sintaxis debe ser: program n L\n  program es el nombre del ejecutable\n  n es el tamaño de la cadena a generar\n  L es la letra de la que se quiere contar apariciones (A, C, G o T)\n");
        exit(1);
    }

    int i, n, count=0, suma=0;
    char *cadena;
    char L;

    n = atoi(argv[1]);
    L = *argv[2];
    cadena = (char *) malloc((n+1)*sizeof(char));
    inicializaCadena(cadena, n);

    if(MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD)!=MPI_SUCCESS){
        return -1;
    };
    if(MPI_Bcast(&L, 1, MPI_CHAR, 0, MPI_COMM_WORLD)!=MPI_SUCCESS){
        return -1;    
    }

    for(i=rank; i<n; i+=numprocs){
        if(cadena[i] == L){
            count++;
        }
    }

    if(MPI_Reduce(&count, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD)!=MPI_SUCCESS){
        return -1;    
    }

    if(rank==0){
        printf("La cadena es %s\n", cadena);
        printf("El numero de apariciones de la letra %c es %d\n", L, suma);
    }
    MPI_Finalize();
    free(cadena);
    exit(0);
}
