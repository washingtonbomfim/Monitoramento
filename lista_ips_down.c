#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista_ips_down.h"
#include "cria_arquivo_conf.h"


int busca_id_mensagem(){
    return 1;
}

char* busca_hora(){
    return "11/10/2017 03:08:00";
}

void inicia(lista *LISTA){
	LISTA->prox = NULL;
}

void adiciona_ips_down(lista *LISTA, char *ip){ 
    printf("adiciona_ips_down: adicionando o ip %s pela primeira vez na lista\n",ip);
    lista *novo=(lista *) malloc(sizeof(lista));
    novo->ip = ip;
    novo->quedas = 1;
    novo->prox = NULL;
    lista *oldHead = LISTA->prox;
    LISTA->prox = novo;
    novo->prox = oldHead;    
}

int busca_ip_down(char *ip, lista *ini){ 

   if (ini->prox == NULL){
       printf("busca_ip_down: Primeira vez, lista é nula\n");
      return 0;
   }
   
   if (strcmp(ini->prox->ip, ip)==0){
       printf("ips da lista %s\n",ini->prox->ip);
       printf("busca_ip_down: achou o ip %s dentro da lista\n", ip);
      return ini->prox->quedas;
   }   
    
   return busca_ip_down(ip, ini->prox); 
}

void atualiza_quedas_ip(char *ip, lista *ini){ 
    printf("atualiza_quedas_ip: atualizando quedas do ip %s \n",ip);
    if (ini->prox == NULL){
      return;
   }
   
   if (strcmp(ini->prox->ip, ip)==0){
      ini->prox->quedas += 1;
   }   
    
   atualiza_quedas_ip(ip, ini->prox); 
}

int vazia(lista *LISTA){
    if(LISTA->prox == NULL){
        return 1;
    }else{
        return 0;
    }    
}

void exibe(lista *LISTA){
    if(vazia(LISTA)){
	printf("Lista vazia!\n\n");
	return ;
    }

    lista *tmp;
    tmp = LISTA->prox;

    while( tmp != NULL){
    	printf("%s", tmp->ip);
	printf("%5d", tmp->quedas);
	tmp = tmp->prox;
    }
    printf("\n\n");
}