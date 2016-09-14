/*Jan Hodermarsky, 2011*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>

#include "enviar_email.h"

void sendsock(u_int sock, u_char* format, ...);

int envia_email(char *mensagem) {
	
    struct sockaddr_in home;
    struct hostent *hp;
    u_char buffer[2048], sockbuf[2048], tmp1[4096],  tmp2[4096];
    u_int sd, delay = 3;
    
    if (!(hp = gethostbyname(SMTP))) {
		fatal("Não foi Possivel resolver ip.\n");
		return 1;
    }
    
    memset((u_char *)&home, 0, sizeof(home));
    home.sin_family = AF_INET;
    home.sin_port = htons(PORT);
    memcpy((u_char *)&home.sin_addr, hp->h_addr, hp->h_length);
        
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Não Foi possivel abrir soket para enviar Email.\n");
		return 1;
    }
    
    if (connect(sd, (struct sockaddr *)&home, sizeof(home)) < 0) {
    	fatal("Não Foi possivel se conectar ao servidor de Email.\n");
    	return -1;
    }

    clean(sockbuf);
    if (recv(sd, sockbuf, sizeof(sockbuf), 0) < 0)
    {
		fatal("recv() error\n");
		return 1;
    }
    
    if (sockbuf[0] != '2'){ /* if we didnt recieve MTA banner quit then */
		fatal("Baniram nois :(\n");
		return 1;
    }
    
    fprintf(stdout, "[+] Connected: %s", sockbuf);
    clean(sockbuf);

    sendsock(sd, "EHLO boy\r\n");
    sendsock(sd, "AUTH LOGIN\r\n");
    encode(strlen(login), login, 128, tmp1); //Base64 encode login
    sendsock(sd, "%s\r\n", tmp1);
    encode(strlen(password), password, 128, tmp2); //Base64 encode login
    sendsock(sd, "%s\r\n", tmp2);

    sleep(delay);
    sendsock(sd, "MAIL FROM: <%s>\r\n", FROM);
    sendsock(sd, "RCPT TO: <%s>\r\n", RECIPIENT);
    recv(sd, sockbuf, sizeof(sockbuf), 0);
    printf("%s\n", sockbuf);
    if (sockbuf[0] == '5') {
        fatal("[!] Someting goes wrong: %s\n", sockbuf);
        return 1;
    }
    clean(sockbuf);
    sendsock(sd, "DATA\r\n");
	/* Is relaying allowed here ? */
    recv(sd, sockbuf, sizeof(sockbuf), 0);
    if (sockbuf[0] == '5') {
        fatal("[!] Something goes wrong: %s", sockbuf);
        return 1;
    }
    clean(sockbuf);
	
    sleep(delay);

    sendsock(sd, "%s\r\n", mensagem);
    sendsock(sd, ".\r\n");
    sleep(delay);

    recv(sd, sockbuf, sizeof(sockbuf), 0);
    printf("%s\n", sockbuf);
    printf("ate aui ok\n");
    
    sleep(delay);
    
    sendsock(sd, "QUIT\r\n");
    return 0;
}

void sendsock(u_int sock, u_char* format, ...) {
    char buf[4096];
    va_list args;
    va_start(args, format);
    clean(buf);
    vsnprintf(buf, sizeof(buf), format, args);
    write(sock, buf, strlen(buf));
    va_end(args);
}

int encode(unsigned s_len, char *src, unsigned d_len, char *dst){
    unsigned triad;
    for (triad = 0; triad < s_len; triad += 3){
        unsigned long int sr;
        unsigned byte;

        for (byte = 0; (byte<3)&&(triad+byte<s_len); ++byte){
            sr <<= 8;
            sr |= (*(src+triad+byte) & 0xff);
        }

        sr <<= (6-((8*byte)%6))%6; /*shift left to next 6bit alignment*/

        if (d_len < 4) return 1; /* error - dest too short */

        *(dst+0) = *(dst+1) = *(dst+2) = *(dst+3) = '=';
        switch(byte){
            case 3:
                *(dst+3) = base64[sr&0x3f];
                sr >>= 6;
            case 2:
                *(dst+2) = base64[sr&0x3f];
                sr >>= 6;
            case 1:
                *(dst+1) = base64[sr&0x3f];
                sr >>= 6;
                *(dst+0) = base64[sr&0x3f];
        }
        dst += 4; d_len -= 4;
    }

    return 0;
}