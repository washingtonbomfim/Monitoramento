#ifndef CRIA_ARQUIVO_CONF_H
#define	CRIA_ARQUIVO_CONF_H

#define ARQ_CONF "ip.conf"
#define ARQ_LOG  "monitoramento.log"
#define ARQ_TEL  "telefone.conf"

void verifica_arquivos();
void adiciona_telefone(char *telefone);
void adiciona_ip_monitoramento(char *ip);
void exibe_ip_configurado();
void exibe_numero_configurado();
void gera_log(char *relatorio);
int preprara_envio_sms(char *relatorio);

#endif	/* CRI_ARQUIVO_CONF_H */

