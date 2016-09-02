#/bin/bash
gcc -locilib -L/usr/local/include -lpthread ./build/Release/GNU-Linux-x86/conexao.o ./build/Release/GNU-Linux-x86/main.o ./build/Release/GNU-Linux-x86/monitoramento.o -o ./monitoramento;
clear;
#rm -rf monitoramento.conf;

./monitoramento -a 192.168.2.103;
./monitoramento -a 192.168.2.1;
./monitoramento -a 192.168.2.104;
./monitoramento -a 192.168.2.105;
#./monitoramento start;
