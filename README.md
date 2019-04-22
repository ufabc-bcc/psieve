# psieve

Psieve is a parallel implementation of the Sieve of Eratosthenes

## Relatório

O relatório está disponível dentro deste projeto e pode ser acessado [aqui](https://github.com/ufabc-bcc/psieve/blob/master/doc/Crivo%20de%20Erat%C3%B3stenes.pdf).

## Compile

Na raiz do projeto execute o seguinte comando

```bash
mpicc src/main.c -g -Wall -o psieve.out
```

## Rode

```bash
mpirun -n <comm_sz> psieve.out <upper_limit>
```

## Avalie o tempo de execução

```bash
time mpirun -n <comm_sz> psieve.out <upper_limit>
```
