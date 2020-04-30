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
#include "server.h"

#define TAM_NAME 31
#define TAM_MSG 101
#define TAM_SEND 201

int main(){
    //-------Sinaliza o estado, caso seja pertinente encerrar o servidor-------
    signal(SIGINT, encerraServidor);

    //-------criando socket-------
    servidor_sockt = socket(AF_INET , SOCK_STREAM , 0);
    if (servidor_sockt == -1) {
        printf("Falha ao criar o socket.");
        exit(EXIT_FAILURE);
    }

    //-------verificação e declaração de variáveis do socket----
    struct sockaddr_in servidor, cliente;
    int s_addrlen = sizeof(servidor);
    int c_addrlen = sizeof(cliente);
    printf("Estabeleça um valor para a porta do servidor: ");
    scanf("%d", &porta);
    memset(&servidor, 0, s_addrlen);
    memset(&cliente, 0, c_addrlen);
    servidor.sin_family = PF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(porta);

    //-------Inicialização do monitor para escrita no arquivo----
    inicializarMonitor(&monitor);

    bind(servidor_sockt, (struct sockaddr *)&servidor, s_addrlen);
    listen(servidor_sockt, 5);

   //-------mensagem do servidor com o valor da porta----
    getsockname(servidor_sockt, (struct sockaddr*) &servidor, (socklen_t*) &s_addrlen);
    printf("Servidor online: %s:%d\n", inet_ntoa(servidor.sin_addr), ntohs(servidor.sin_port));

    //-------Lista de usuários iniciada----
    inicio = novo(servidor_sockt, inet_ntoa(servidor.sin_addr), ntohs(servidor.sin_port));
    atual = inicio;

    while(1){
        cliente_sockt = accept(servidor_sockt, (struct sockaddr*) &cliente, (socklen_t*) &c_addrlen);

        //-------Informa quando um novo cliente acessou o servidor com a respectiva porta----
        getpeername(cliente_sockt, (struct sockaddr*) &cliente, (socklen_t*) &c_addrlen);
        printf("Novo cliente com IP %s:%d se conectou.\n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));
        sprintf(monitor.informacao, "Novo cliente com IP %s:%d se conectou.\n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));

        if(pthread_create(&threadMonitor, NULL, (void*)inserirLinha, (void*)&monitor) != 0){
            printf ("Erro ao criar thread!\n");
            exit(EXIT_FAILURE);
        }

        //-------Insere um novo cliente a lisat dos já existentes----
        TCliente *aux = novo(cliente_sockt, inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));
        aux->prev = atual;
        atual->link = aux;
        atual = aux;

        //-------Thread para manipulação do novo cliente-------
        pthread_t thread;
        if(pthread_create(&thread, NULL, (void*)manipulaCliente, (void *)aux) != 0){
            printf ("Erro ao criar thread!\n");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
