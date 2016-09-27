#ifndef MONITORAMENTO_H
#define	MONITORAMENTO_H

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

#define PACKETSIZE	64

struct sockaddr_in monta_cabecalho_protocolo(char *ip);
void busca_ip_configurado();
int abre_socket();
void ping(struct sockaddr_in *addr, char *ip, int sk);
int checksum(void *b, int len);

#endif	/* MONITORAMENTO_H */

