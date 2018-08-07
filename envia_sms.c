#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "envia_sms.h"
#include "cria_arquivo_conf.h"
#include <json-c/json.h>
#include <curl/curl.h>

#define PRIVATE
#define PUBLIC extern

struct curl_fetch_st {
    char *payload;
    size_t size;
};

PRIVATE size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct curl_fetch_st *p = (struct curl_fetch_st *) userp;

    p->payload = (char *) realloc(p->payload, p->size + realsize + 1);

    if (p->payload == NULL) {
      gera_log("ERROR: Failed to expand buffer in curl_callback");
      free(p->payload);
      return -1;
    }

    memcpy(&(p->payload[p->size]), contents, realsize);

    p->size += realsize;

    /* ensure null termination */
    p->payload[p->size] = 0;

    /* return size */
    return realsize;
}

PRIVATE CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch){
    
    CURLcode rcode;                   /* curl result code */
    /* init payload */
    fetch->payload = (char *) calloc(1, sizeof(fetch->payload));

    /* check payload */
    if (fetch->payload == NULL) {
        /* log error */
        gera_log("ERROR: Failed to allocate payload in curl_fetch_url");
        /* return error */
        return CURLE_FAILED_INIT;
    }

    /* init size */
    fetch->size = 0;

    /* set url to fetch */
    curl_easy_setopt(ch, CURLOPT_URL, url);

    /* set calback function */
    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);

    /* pass fetch struct pointer */
    curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);

    /* set default user agent */
    curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* set timeout */
    curl_easy_setopt(ch, CURLOPT_TIMEOUT, 5);

    /* enable location redirects */
    curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);

    /* set maximum allowed redirects */
    curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

    /* fetch the url */
    rcode = curl_easy_perform(ch);

    /* return */
    return rcode;
}

PUBLIC int envia_sms(Mensagem Msg){
    CURL *ch;                        
    CURLcode rcode;
    char *string_url = malloc(sizeof(char)*500);
    json_object *json;                                      
    enum json_tokener_error jerr = json_tokener_success;

    struct curl_fetch_st curl_fetch;
    struct curl_fetch_st *cf = &curl_fetch;
    struct curl_slist *headers = NULL;

    char *url = "https://api-rest.zenvia360.com.br/services/send-sms";

    if ((ch = curl_easy_init()) == NULL) {
        gera_log("ERROR: nao conseguiu fazer o curl_easy_init()");
        return 1;
    }
    headers = curl_slist_append(headers, "Authorization: Basic YnJhc2lsY2FyZC5yZXN0OlBNMFFIcnl5ZmU=");
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");    

    json = json_object_new_object();
    fflush(stdout);
    sprintf(string_url, "{\"sendSmsRequest\": { \"from\": \"Brasilcard\", \"to\": \"%s\",\"schedule\": \"%s\",\"msg\": \"%s \",\"callbackOption\": \"NONE\",\"id\": \"'%d'\",\"aggregateId\": \"1111\"}}\n", Msg.telefone,Msg.dia,Msg.msg,Msg.id_msg);
    printf("%s\n",string_url);
    json = json_tokener_parse(string_url);
    curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(ch, CURLOPT_POSTFIELDS, json_object_to_json_string(json));

    rcode = curl_fetch_url(ch, url, cf);

    curl_easy_cleanup(ch);

    curl_slist_free_all(headers);

    json_object_put(json);

    if (rcode != CURLE_OK || cf->size < 1) {
        fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s",url, curl_easy_strerror(rcode));
        return 2;
    }

    if (cf->payload != NULL) {
        printf("CURL Returned: \n%s\n", cf->payload);
        json = json_tokener_parse_verbose(cf->payload, &jerr);
        free(cf->payload);
    } else {
        gera_log("ERROR: Failed to populate payload");
        free(cf->payload);
        return 3;
    }

    if (jerr != json_tokener_success) {
        gera_log("ERROR: Failed to parse json string");
        json_object_put(json);
        return 4;
    }

    //printf("Parsed JSON: %s\n", json_object_to_json_string(json));

    return 0;
}