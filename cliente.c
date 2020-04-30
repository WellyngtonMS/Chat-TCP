#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "cliente.h"

#define TAM_NAME 31
#define TAM_MSG 101
#define TAM_SEND 201


int main(){
    //-------Sinaliza o estado, caso seja pertinente à saida do cliente-------
    signal(SIGINT, sinalizaSaida);

    //-------verificação do nome de usuario-------
    char nome[TAM_NAME] = {};
    printf("Digite seu nome: ");
    if(fgets(nome, TAM_NAME, stdin) != NULL){
        retira(nome, TAM_NAME);
    }
    if(strlen(nome) < 2){
        printf("\nTamanho do nome muito curto.\n");
        exit(EXIT_FAILURE);
    }
    if(strlen(nome) >= TAM_NAME-1){
        printf("\nExcedeu o limite de caracteres.\n");
        exit(EXIT_FAILURE);
    }

    //-------criando socket-------
    sockt = socket(AF_INET , SOCK_STREAM , 0);
    if(sockt == -1){
        printf("Falha ao criar o socket.");
        exit(EXIT_FAILURE);
    }

    //-------verificação e declaração de variáveis do socket----
    struct sockaddr_in servidor, cliente;
    int s_addrlen = sizeof(servidor);
    int c_addrlen = sizeof(cliente);
    int porta;
    char ip[1024];
    printf("Digite o numero da porta de acesso do servidor: ");
    scanf("%d", &porta);
    printf("Digite o numero do seu IP: ");
    scanf("%s", ip);
    memset(&servidor, 0, s_addrlen);
    memset(&cliente, 0, c_addrlen);
    servidor.sin_family = PF_INET;
    servidor.sin_addr.s_addr = inet_addr(ip);
    servidor.sin_port = htons(porta);

    //-------Conexão do servidor----
    int err = connect(sockt, (struct sockaddr *)&servidor, s_addrlen);
    if(err == -1){
        printf("Acesso negado ao servidor ou numero incorreto!\n");
        exit(EXIT_FAILURE);
    }else{
        printf("Acesso autorizado ao servidor!\n");
    }

    //-------Informações capturadas ----
    getsockname(sockt, (struct sockaddr*) &cliente, (socklen_t*) &c_addrlen);
    getpeername(sockt, (struct sockaddr*) &servidor, (socklen_t*) &s_addrlen);
    printf("Conectado ao servidor com o IP %s:%d na porta %d.\n", inet_ntoa(servidor.sin_addr), ntohs(cliente.sin_port), ntohs(servidor.sin_port));
    send(sockt, nome, TAM_NAME, 0);

    //-------Threads para envio e recebimento de mensagem ao cliente-------
    pthread_t threadsend, threadrecv;
    if(pthread_create(&threadsend, NULL, (void *) enviaMensagem, NULL) != 0){
        printf ("Erro ao criar thread!\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&threadrecv, NULL, (void *) recebeMensagem, NULL) != 0){
        printf ("Erro ao criar thread!\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        if(flag){
            printf("\nAte mais!\n");
            break;
        }
    }

    close(sockt);
    return 0;
}
