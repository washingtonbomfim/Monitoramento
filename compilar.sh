#/bin/bash
gcc -ljson-c -locilib -L/usr/local/include -lpthread -lcurl ./build/Release/GNU-Linux-x86/conexao.o ./build/Release/GNU-Linux-x86/main.o ./build/Release/GNU-Linux-x86/monitoramento.o  ./build/Release/GNU-Linux-x86/enviar_email.o ./build/Release/GNU-Linux-x86/envia_sms.o ./build/Release/GNU-Linux-x86/cria_arquivo_conf.o ./build/Release/GNU-Linux-x86/cria_daemon.o -o ./monitoramento
#clear;
#rm -rf monitoramento.conf;

#./monitoramento -a 192.168.2.75;
#./monitoramento -a 192.168.5.8;
#./monitoramento start;
