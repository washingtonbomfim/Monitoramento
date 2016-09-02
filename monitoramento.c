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

struct packet {
    struct icmphdr hdr;
    char msg[PACKETSIZE - sizeof (struct icmphdr)];
};

int pid = -1;
struct protoent *protocolo = NULL;

void verifica_arquivo(){
    
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
}

void adiciona_ip_monitoramento(char *ip){
    
    verifica_arquivo();
    FILE *arquivo;
    arquivo = fopen(ARQ_CONF,"r");
    
    if(!arquivo){
        printf("Ocorreu um erro ao abrir/criar arquivo onde fica salvo os Ips de monitoramento.\n");
    }
    fclose(arquivo);
    
    arquivo = fopen(ARQ_CONF,"a");
    fprintf(arquivo,"%s\n",ip);
    printf("Ip Adciononado, %s .\n",ip);
    fclose(arquivo);
    //exibe_ip_configurado();
    
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

void busca_ip_configurado(){

    FILE *arquivo;
    char buffer[500], *ip;
    int x = 0, flag = 0, qtd_thread = 0;
    struct sockaddr_in addr;
    
    arquivo = fopen(ARQ_CONF,"r");
    
    bzero(&buffer, sizeof(buffer));
    
    if(!arquivo){
        printf("Não foi possivel listar os Ips\n");
        exit(1);
    }
    while(flag==0){
        
        buffer[x]=fgetc(arquivo);
        
        if(buffer[x] == EOF){
            flag = 1;
            wait(0);
        }        
        if(buffer[x] == '\n'){
            buffer[x] = '\0';
            addr = inicia_monitoramento(buffer);
            ping(&addr,buffer);
            x=-1;
        }
        x++;
    }
    wait(0);
    fclose(arquivo);
}

struct sockaddr_in inicia_monitoramento(char *ip){
    
    struct hostent *hname;
    struct sockaddr_in addr;
    pid = getpid();
    protocolo = getprotobyname("ICMP");
    hname = gethostbyname(ip);
    bzero(&addr, sizeof(addr));
    addr.sin_family = hname->h_addrtype;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = *(long*) hname->h_addr;

    return addr;     
}

int abre_socket(){
    
    int sd;
    const int val = 255;
    
    sd = socket(PF_INET, SOCK_RAW, 1);
    if (sd < 0) {
        perror("Não foi possivel abrir socket\n");
        return;
    }

    if (setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof (val)) != 0){
        perror("Set TTL option");
    }        

    if (fcntl(sd, F_SETFL, O_NONBLOCK) != 0){
        perror("Request nonblocking I/O");
    }
    
    return sd;
}

void ping(void *cabecalho, char *ip) {
    
    struct sockaddr_in *addr = (struct sockaddr_in *) cabecalho;
    int i,sd, qtd_ping = 0, quedas = 0;
    struct packet pckt;
    struct sockaddr_in r_addr;
    
    sd = abre_socket();
    
    for ( qtd_ping ; qtd_ping <= 5; qtd_ping++) {
        int len = sizeof (r_addr);
        if (recvfrom(sd, &pckt, sizeof (pckt), 0, (struct sockaddr*) &r_addr, &len) <=0){
            if(qtd_ping == 0){
                printf("Verificação da Vpn %s:\n", ip);
            }else{
                printf(RED "Ping ERRO" RESET "\n");
                gera_log(ip);
                quedas++;
            }            
        }else{
            printf(GREEN "Ping OK" RESET "\n");
        }        
        
        bzero(&pckt, sizeof (pckt));
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = pid;
        for (i = 0; i < sizeof (pckt.msg) - 1; i++){
            pckt.msg[i] = i + '0';
        }
           
        pckt.msg[i] = 0;
        pckt.hdr.un.echo.sequence = qtd_ping;
        pckt.hdr.checksum = checksum(&pckt, sizeof (pckt));

        if (sendto(sd, &pckt, sizeof (pckt), 0, (struct sockaddr*) addr, sizeof (*addr)) <= 0) {
            perror("sendto");
        }
        sleep(1);
    }
    
    if(quedas++){
        printf(YELLOW "Enviado email sobre informações de queda" RESET ".\n");
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

void gera_log(char *ip){
 
    verifica_arquivo();
    FILE *arquivo;
    arquivo = fopen(ARQ_LOG,"r");
    
    if(!arquivo){
        printf("Ocorreu um erro ao abrir/criar arquivo onde fica salvo os logs de erro.\n");
        return;
    }
    
    arquivo = fopen(ARQ_LOG,"a");
    fprintf(arquivo,"Data :%s Hora:%s - IP %s\n",__DATE__,__TIME__, ip);
    fclose(arquivo);
}
