#ifndef LISTA_IPS_DOWN_H
#define	LISTA_IPS_DOWN_H

struct ListaIps{
    char *ip;
    int quedas;
    struct ListaIps *prox;
};

typedef struct ListaIps lista;

int busca_id_mensagem();
char* busca_hora();
void inicia(lista *LISTA);
void adiciona_ips_down(lista *LISTA, char *ip);
int busca_ip_down(char *ip, lista *ini);
void atualiza_quedas_ip(char *ip, lista *ini);
void exibe(lista *LISTA);

#endif	/*LISTA_IPS_DOWN_H*/

