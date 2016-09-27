#include <stdio.h>
#include <stdlib.h>
#include <ocilib.h>
#include "conexao.h"
#include "cria_arquivo_conf.h"

int busca_id_mensagem(){
    
    OCI_Connection *cn;
    OCI_Statement* st;
    OCI_Resultset* rs;
    char *id_char;
    int id = 0;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT)){
        return EXIT_FAILURE;
    }
              
    cn = OCI_ConnectionCreate("", "", "", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    OCI_ExecuteStmt(st, "SELECT SMS_ID_REST.NEXTVAL AS Id FROM DUAL");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
         id_char = OCI_GetString(rs, 1);
         id = atoi(id_char);
    }
    
    OCI_Cleanup();
    return id;
}

char* busca_hora(){

    OCI_Connection *cn;
    OCI_Statement* st;
    OCI_Resultset* rs;
    char *dia = malloc(sizeof(char)*20);
    
    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT)){
        gera_log("erro ao iniciar conexao com oracle");
        return "c";
    }
              
    cn = OCI_ConnectionCreate("", "", "", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    OCI_ExecuteStmt(st, "SELECT TO_CHAR(SYSDATE,'YYYY-MM-DD')||'T'|| TO_CHAR(SYSDATE,'HH:MI:SS') FROM DUAL");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs)){
         strncpy(dia, OCI_GetString(rs, 1),20);
    }
    dia[20]='\0';
    OCI_Cleanup();
    return dia;
    
}
