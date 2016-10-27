#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "monitoramento.h"
#include "cria_arquivo_conf.h"
#include "conexao.h"
#include "envia_sms.h"
#include "enviar_email.h"

struct packet {
    struct icmphdr hdr;
    char msg[PACKETSIZE - sizeof(struct icmphdr)];
};

int pid = -1, quedas_total = 0;
struct protoent *protocolo = NULL;

struct sockaddr_in monta_cabecalho_protocolo(char *ip){
    
    struct hostent *hname;
    struct sockaddr_in addr;
    pid = getpid();
    protocolo = getprotobyname("ICMP");
    hname = gethostbyname(ip);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = hname->h_addrtype;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = *(long*) hname->h_addr;

    return addr;     
}

void busca_ip_configurado(){

    FILE *arquivo;
    char buffer[500], *ip;
    int x = 0, qtd_thread = 0, sd = 0;
    struct sockaddr_in addr;
    
    arquivo = fopen(ARQ_CONF,"r");   
    memset(&buffer, 0, sizeof(buffer));
    
    if(!arquivo){
        gera_log("Não foi possivel listar os Ips");
        exit(1);
    }
    while(1){
        
        buffer[x]=fgetc(arquivo);
        
        if(buffer[x] == EOF){
            rewind(arquivo);
            x=0;
            buffer[x]=fgetc(arquivo);
        }
        if(buffer[x] == '\n'){
            buffer[x] = '\0';
            addr = monta_cabecalho_protocolo(buffer);
            sd = abre_socket();
            quedas_total = 0;
            ping(&addr,buffer, sd);
            x=-1;
        }
        x++;
    }
    wait(0);
    fclose(arquivo);
}

int abre_socket(){
    
    int sd;
    const int val = 255;
    
    sd = socket(PF_INET, SOCK_RAW, 1);
    if (sd < 0) {
        gera_log("Não foi possivel abrir socket");
        return;
    }

    if (setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof (val)) != 0){
        gera_log("Set TTL option");
    }        

    if (fcntl(sd, F_SETFL, O_NONBLOCK) != 0){
        gera_log("Request nonblocking I/O");
    }
    
    return sd;
}

void ping(struct sockaddr_in *addr, char *ip, int sk) {
    
    int i, qtd_ping = 0, quedas = 0, t = 0;
    struct packet pckt;
    struct sockaddr_in r_addr;
    char mensagem[50];
    
    for ( qtd_ping ; qtd_ping <= 3; qtd_ping++) {
        int len = sizeof (r_addr);
        t = recvfrom(sk, &pckt, sizeof(pckt), 0, (struct sockaddr*) &r_addr, &len);
        if ( t <= 0 || pckt.msg[i] != 'S'){
            if(qtd_ping == 0){
                printf("Verificação da Vpn %s:\n", ip);
            }else{
                printf(RED "Ping ERRO" RESET "\n");
                quedas++;
            }            
        }else{
            printf(GREEN "Ping OK" RESET "\n");
        }        
        
        memset(&pckt, 0, sizeof (pckt));
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = pid;
        for (i = 0; i < sizeof(pckt.msg) - 1; i++){
            pckt.msg[i] = i + '0';
        }
           
        pckt.msg[i] = 0;
        pckt.hdr.un.echo.sequence = qtd_ping;
        pckt.hdr.checksum = checksum(&pckt, sizeof (pckt));

        if (sendto(sk, &pckt, sizeof(pckt), 0, (struct sockaddr*) addr, sizeof(*addr)) <= 0) {
            gera_log("Erro ao enviar pacote.");
        }
        sleep(1);
    }
    
    if(quedas_total == 3){
        gera_log("Enviado email sobre informações de queda.");
        sprintf(mensagem,"Queda do Ip %s. Favor verificar.",ip);
        verifica_quedas(ip, mensagem);
        gera_log(mensagem);
        close(sk);
        return;
    }
    
    if(quedas == 3){
        quedas_total++;
        printf("Tentativa de Ping %d de 3... \n",quedas_total);
        struct sockaddr_in addrb = monta_cabecalho_protocolo(ip);
        ping(&addrb, ip, sk);
    }
    close(sk);
}

void verifica_quedas(char *ip, char *mensagem){
    
    int verifica = 0;
    
    verifica = busca_ip_down(ip);
    if(verifica == 0){
        adiciona_ips_down(ip);
        //envia_email(mensagem);
        //preprara_envio_sms(mensagem);
    }else if(verifica == 2){
        //envia_email(mensagem);
        //preprara_envio_sms(mensagem);
    }   

}

int checksum(void *b, int len){

    unsigned short *buf = b;
    unsigned int sum = 0;
    int result;
    
    for (sum = 0; len > 1; len -= 2){
        sum += *buf++;
    }
    
    if (len == 1){
        sum += *(unsigned char*) buf;
    }    

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;

    return result;
}
