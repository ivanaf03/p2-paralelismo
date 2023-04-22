# p2-paralelismo
Práctica 2 de Paralelismo. Hay un par de errores. L y n están en todos los procesos. Se debe hacer como en la prática 1 (fue un error) y además el BinomialBcast no está correcto. Se puede arreglar así:
```
  if(rank!=0) recv
  for(i=rank+1;i<numprocs;i++)
    hijo=1<<i
    if(rank+hijo<numprocs)
      send
    else
      break
```
      
