#include <stdio.h>
#include "cria_arquivo_conf.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "envia_sms.h"
#include "conexao.h"

void verifica_arquivos(){
    
    FILE *arquivo;
    arquivo = fopen(ARQ_CONF,"r");
       
    if(!arquivo){
        arquivo = fopen(ARQ_CONF,"wb");
        fclose(arquivo);
    }else{
        fclose(arquivo);
    }
    
    arquivo = fopen(ARQ_LOG,"r");
    
    if(!arquivo){
        arquivo = fopen(ARQ_LOG,"wb");
        fclose(arquivo);
    }else{
        fclose(arquivo);
    }
    
    arquivo = fopen(ARQ_TEL,"r");
    
    if(!arquivo){
        arquivo = fopen(ARQ_TEL,"wb");
        fclose(arquivo);
    }else{
        fclose(arquivo);
    }
    
}

void adiciona_telefone(char *telefone){
    
    verifica_arquivos();
    FILE *arquivo;
    arquivo = fopen(ARQ_TEL,"r");
    
    if(!arquivo){
        printf("Ocorreu um erro ao abrir/criar arquivo onde fica salvo os telefones para envio das mensagens.\n");
    }
    fclose(arquivo);
    
    arquivo = fopen(ARQ_TEL,"a");
    fprintf(arquivo,"%s\n",telefone);
    printf("Telefone Adciononado, %s.\n",telefone);
    fclose(arquivo);
}

void adiciona_ip_monitoramento(char *ip){
    
    verifica_arquivos();
    FILE *arquivo;
    arquivo = fopen(ARQ_CONF,"r");
    
    if(!arquivo){
        printf("Ocorreu um erro ao abrir/criar arquivo onde fica salvo os Ips de monitoramento.\n");
    }
    fclose(arquivo);
    
    arquivo = fopen(ARQ_CONF,"a");
    fprintf(arquivo,"%s\n",ip);
    printf("Ip Adciononado, %s.\n",ip);
    fclose(arquivo);
    
}

void exibe_ip_configurado(){
    
    FILE *arquivo;
    char buffer;
    arquivo = fopen(ARQ_CONF,"r");
    
    if(!arquivo){
        printf("Não foi possivel listar os Ips.\n");
        exit(1);
    }
    
    while((buffer=fgetc(arquivo))!= EOF){
        putchar(buffer);
    }
    fclose(arquivo);
}
void exibe_numero_configurado(){
    
    FILE *arquivo;
    char buffer;
    arquivo = fopen(ARQ_TEL,"r");
    
    if(!arquivo){
        printf("Não foi possivel listar os Numeros.\n");
        exit(1);
    }
    
    while((buffer=fgetc(arquivo))!= EOF){
        putchar(buffer);
    }
    fclose(arquivo);
}

void gera_log(char *relatorio){
 
    verifica_arquivos();
    FILE *arquivo;
    arquivo = fopen(ARQ_LOG,"r");
    int x = 0;
    char *hora = malloc(sizeof(char)*20);
    
    if(!arquivo){
        printf("Ocorreu um erro ao abrir/criar arquivo onde fica salvo os logs de erro.\n");
        return;
    }
    
    strncpy(hora,busca_hora(),20);
    
    arquivo = fopen(ARQ_LOG,"a");
    fprintf(arquivo,"Horario : %s Mensagem = %s\n",hora,relatorio);
    fclose(arquivo);
}

int preprara_envio_sms(char *relatorio){
    
    Mensagem msg;
    DATA data;
    FILE *arquivo;
    char buffer[500];
    int x = 0;
    int i;
    msg.dia = malloc(sizeof(char)*22);
    msg.msg     = relatorio;
    strncpy(msg.dia, busca_hora(),20);
    msg.dia[20] = '\0';
    arquivo     = fopen(ARQ_TEL,"r");
    
    if(!arquivo){
        gera_log("Não foi possivel abrir arquivos com telefones");
        exit(1);
    }
    
    while(1){
        buffer[x]=fgetc(arquivo);
        if(buffer[x] == EOF){
            return 0;
        }
        if(buffer[x] == '\n'){
            buffer[x] = '\0';
            msg.id_msg  = busca_id_mensagem();
            msg.telefone = buffer;
            envia_sms(msg);
            x = -1;
        }
        x++;
    }
    wait(0);
    fclose(arquivo);    
    return 0;    
}