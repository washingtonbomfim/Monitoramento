#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conexao.h"
#include "monitoramento.h"

void ajuda();

int main(int argc, char** argv) {
    
    if(argc < 2){
        ajuda();
    }
    
    if(strcmp(argv[1],"-a")==0){
        if(argv[2] == NULL){
            printf(RED "Falta argumento para -a\n" RESET);
            ajuda();
        }
        adiciona_ip_monitoramento(argv[2]);
        return 0;
    }
    
    if(strcmp(argv[1],"-m")==0){
        exibe_ip_configurado();
        return 0;
    }
    
    if(strcmp(argv[1],"start")==0){
        busca_ip_configurado();
        //inicia_monitoramento(argv[1]);
    }  
    return (0);
}

void ajuda(){    
    printf("-a          *** Adicionar ip para ser monitorado.Ex: -a 192.168.1.10\n");
    printf("-m          *** Mostra Ips configurados.\n");
    exit(0);
}