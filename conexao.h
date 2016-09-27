#ifndef CONEXAO_H
#define	CONEXAO_H

typedef struct sysdate {
    char *dia;
    char *hora;
    int status;
}DATA;

int busca_id_mensagem();
char* busca_hora();

#endif	/* CONEXAO_H */

