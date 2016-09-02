#include <stdio.h>
#include <stdlib.h>
#include <ocilib.h>
#include "conexao.h"

char* oracle(){
    
    OCI_Connection *cn;
    OCI_Statement* st;
    OCI_Resultset* rs;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT)){
        return EXIT_FAILURE;
    }
            
    char *hora;
    cn = OCI_ConnectionCreate("EX01", "washington", "bomfim159753", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    OCI_ExecuteStmt(st, "select sysdate from dual");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
         printf("%s\n",OCI_GetString(rs, 1));
         hora = OCI_GetString(rs, 1);
         
    }
    
    OCI_Cleanup();

    return hora;
}