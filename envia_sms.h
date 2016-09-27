#ifndef ENVIA_SMS_H
#define	ENVIA_SMS_H

typedef struct mensagem{
    char *telefone;
    char *msg;
    int id_msg;
    char *hora;
    char *dia;
} Mensagem;

size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp);
int envia_sms(Mensagem Msg);

#endif