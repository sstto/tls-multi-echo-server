#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet//in.h>
#include <netdb.h>

#include <openssl/rsa.h>
#include <openssl/crypto.h>
//#include <openssl/X509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUF_SIZE 1024
void error_handling(char *message);
SSL_CTX* InitCTX(void)
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL_library_init();
    SSL_load_error_strings();
    method = TLSv1_2_client_method();
    ctx = SSL_CTX_new(method);
    if ( ctx == NULL )
    {
        printf("Faild to create SSL_CTX\n");
        abort();
    }
    return ctx;
}
void ShowCerts(SSL* ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl);
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line); free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line); free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}
int main(int argc, char *argv[]){
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr;
    SSL_CTX *ctx;
    SSL *ssl;

    if(argc != 3){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        error_handling("socket() error");
    }

    ctx = InitCTX();
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1){
        error_handling("connect() error!");
    }else{
        puts("connected...");
    }
    if ( SSL_connect(ssl) == -1)
        printf("Faild to HandShake\n");
    else{
        ShowCerts(ssl);
        while(1){
            fputs("Input message(Q to quit): ", stdout);
            fgets(message, BUF_SIZE, stdin);

            if(!strcmp(message, "q\n") || !strcmp(message, "Q\n")){
                break;
            }

            SSL_write(ssl, message, strlen(message));
            str_len = SSL_read(ssl, message, BUF_SIZE-1);
            message[str_len] = 0;
            printf("Message from server: %s", message);
        }
        SSL_free(ssl);
    }

    close(sock);
    SSL_CTX_free(ctx);
    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
