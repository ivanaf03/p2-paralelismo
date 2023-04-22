//Iván Álvarez Fernández
//Marcos Gómez Tejón

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

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

int MPI_BinomialBcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm){
    int numprocs;
    int rank;
    MPI_Comm_size(comm, &numprocs);
    MPI_Comm_rank(comm, &rank);

    if(comm==NULL){
        return MPI_ERR_COMM;
    }
    if(count<0){
        return MPI_ERR_COUNT;
    }
    if(buffer==NULL){
        return MPI_ERR_BUFFER;
    }
    if(root<0 || root>=numprocs){
        return MPI_ERR_ROOT;
    }
    for(int i=0; i<numprocs; i++){
        if(rank<pow(rank, 1<<i) && rank+pow(rank, 1<<i)<numprocs){ //Procesos con rango < 2^(i-1) y condición de salida
            if(MPI_Send(buffer, count, datatype, pow(rank, 1<<i), 0, comm)!=MPI_SUCCESS){
                return -1;
            }
        } else if(rank>pow(rank, 1<<i)){ //Procesos con rango+2^(i-1)
            if(MPI_Recv(buffer, count, datatype, pow(rank, 1<<i), 0, comm, MPI_STATUS_IGNORE)!=MPI_SUCCESS){
                return -1;
            }
        }
    }
    return MPI_SUCCESS;
}

int MPI_FlattreeColectiva(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm){
    int numprocs;
    int rank;
    MPI_Comm_size(comm, &numprocs);
    MPI_Comm_rank(comm, &rank);
    int *cnt=(int*) sendbuf;
    int suma=0;
    int *ret_value=(int*) recvbuf;

    if(comm==NULL){
        return MPI_ERR_COMM;
    }
    if(count<0){
        return MPI_ERR_COUNT;
    }
    if(datatype!=MPI_INT){
        return MPI_ERR_TYPE;
    }
    if(sendbuf==NULL){
        return MPI_ERR_BUFFER;
    }
    if(root<0 || root>=numprocs){
        return MPI_ERR_ROOT;
    }
    if(op!=MPI_SUM){
        return MPI_ERR_OP;
    }

    if(rank!=root) {
        if(MPI_Send(cnt, count, datatype, root, 0, comm)!=MPI_SUCCESS){
            return -1;
        }

    }else{
        (*ret_value) += (*cnt);
        for (int j = 1; j < numprocs; j++) {
            if(MPI_Recv(&suma, count, datatype, j, 0, comm, MPI_STATUS_IGNORE)!=MPI_SUCCESS){
                return -1;
            }
            (*ret_value) += suma;
        }
    }
    return MPI_SUCCESS;
}

int main(int argc, char *argv[]) {
    int numprocs, rank, sum, ret_value;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(argc != 3){
        printf("Numero incorrecto de parametros\nLa sintaxis debe ser: program n L\n  program es el nombre del ejecutable\n  n es el tamaño de la cadena a generar\n  L es la letra de la que se quiere contar apariciones (A, C, G o T)\n");
        exit(1);
    }

    int i, n, count=0, suma=5;
    char *cadena;
    char L;

    n = atoi(argv[1]);
    L = *argv[2];
    cadena = (char *) malloc((n+1)*sizeof(char));
    inicializaCadena(cadena, n);

    if(MPI_BinomialBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD)!=MPI_SUCCESS){
        return -1;    
    }
    if(MPI_BinomialBcast(&L, 1, MPI_CHAR, 0, MPI_COMM_WORLD)!=MPI_SUCCESS){
        return -1;    
    }

    for(i=rank; i<n; i+=numprocs){
        if(cadena[i] == L){
            count++;
        }
    }
    
    if(MPI_FlattreeColectiva(&count, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD)!=MPI_SUCCESS){
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


