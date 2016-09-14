#/bin/bash
gcc -locilib -L/usr/local/include -lpthread -lcurl ./build/Release/GNU-Linux-x86/conexao.o ./build/Release/GNU-Linux-x86/main.o ./build/Release/GNU-Linux-x86/monitoramento.o  ./build/Release/GNU-Linux-x86/enviar_email.o -o ./monitoramento
#clear;
#rm -rf monitoramento.conf;

#./monitoramento -a 192.168.2.75;
#./monitoramento -a 192.168.5.8;
#./monitoramento start;
