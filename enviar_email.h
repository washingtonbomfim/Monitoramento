#ifndef ENVIAR_EMAIL_H
#define	ENVIAR_EMAIL_H

#define PORT 25
#define FROM "washington@brasilcard.com"
#define RECIPIENT "washington@brasilcard.com"
#define SMTP ""
#define TEXT "teste 1234"

#define fatal(x...)	fprintf(stderr, x)
#define clean(x)	memset(x, '\0', sizeof(x))

static char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789"
"+/";

char login[] = "washington@brasilcard.com";
char password[] = "";

int envia_email(char *mensagem);
void sendsock(u_int sock, u_char* format, ...);
int encode(unsigned s_len, char *src, unsigned d_len, char *dst);

#endif