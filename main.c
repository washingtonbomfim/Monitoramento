#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conexao.h"
#include "cria_daemon.h"
#include "monitoramento.h"
#include "cria_arquivo_conf.h"

void ajuda();

int main(int argc, char** argv) {
    
    if(argc < 2){
        ajuda();
    }
    if(strcmp(argv[1],"-a")==0){
        if(argv[2] == NULL){
            printf(RED "Falta argumento para -a (Adicionar ip)\n" RESET);
            ajuda();
        }
        adiciona_ip_monitoramento(argv[2]);
        return 0;
    }
    if(strcmp(argv[1],"-t")==0){
       if(argv[2] == NULL){
            printf(RED "Falta argumento para -t (Adicionar telefone)\n" RESET);
            ajuda();
        }
       adiciona_telefone(argv[2]);
       return 0;
    }
    if(strcmp(argv[1],"-m")==0){
        printf("************** Ips Cadastrados **************\n");
        exibe_ip_configurado();
        printf("\n************** Numeros Cadastrados **************\n");
        exibe_numero_configurado();
        return 0;
    }    
    if(strcmp(argv[1],"start")==0){
        gera_log("Iniciando Monitoramento");
        daemonize();
        busca_ip_configurado();
    }
    if(strcmp(argv[1],"-h") || strcmp(argv[1],"-help") || strcmp(argv[1],"--help")){
        ajuda();
        return 0;
    }
    return (0);
}

void ajuda(){    
    
    printf("monitoramento -a          *** Adicionar ip para ser monitorado.Ex: -a 192.168.1.10\n");
    printf("monitoramento -t          *** Adicionar telefone para envio de sms. Ex: -t 5564992165920\n");
    printf("monitoramento -m          *** Mostra Ips e Numeros de celulares configurados.\n");
    printf("monitoramento start       *** Inicia o software\n");
    exit(0);
}