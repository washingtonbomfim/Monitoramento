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
    char msg[PACKETSIZE - sizeof(struct icmphdr)];
};

int pid = -1, quedas_total = 0;
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
    int x = 0, qtd_thread = 0, sd = 0;
    struct sockaddr_in addr;
    
    arquivo = fopen(ARQ_CONF,"r");   
    memset(&buffer, 0, sizeof(buffer));
    
    if(!arquivo){
        printf("Não foi possivel listar os Ips\n");
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
            addr = inicia_monitoramento(buffer);
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

struct sockaddr_in inicia_monitoramento(char *ip){
    
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

void ping(struct sockaddr_in *addr, char *ip, int sk) {
    
    int i, qtd_ping = 0, quedas = 0, t = 0;
    struct packet pckt;
    struct sockaddr_in r_addr;
    
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
            perror("Erro ao enviar pacote.\n");
        }
        sleep(1);
    }
    
    if(quedas_total == 3){
        printf(YELLOW "Enviado email sobre informações de queda." RESET "\n");
        gera_log("Enviado email sobre informações de queda.",ip);
        close(sk);
        return;
    }
    
    if(quedas == 3){
        quedas_total++;
        printf("Tentativa de Ping %d de 3... \n",quedas_total);
        struct sockaddr_in addrb = inicia_monitoramento(ip);
        ping(&addrb, ip, sk);
    }
    close(sk);
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

void gera_log(char *relatorio,char *ip){
 
    verifica_arquivo();
    FILE *arquivo;
    arquivo = fopen(ARQ_LOG,"r");
    int x = 0;
    
    if(!arquivo){
        printf("Ocorreu um erro ao abrir/criar arquivo onde fica salvo os logs de erro.\n");
        return;
    }
    
    arquivo = fopen(ARQ_LOG,"a");
    fprintf(arquivo,"Data : %s Hora: %s - IP: %s - Mensagem = %s\n",__DATE__,__TIME__, ip,relatorio);
    fclose(arquivo);
    x = envia_email(relatorio);
}
