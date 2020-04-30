#define TAM_NAME 31
#define TAM_MSG 101
#define TAM_SEND 201

//variáveis globais
volatile sig_atomic_t flag = 0; //variável de sinalização
int sockt = 0;

void retira(char* vetor, int tamanho){
    int i;
    for (i = 0; i < tamanho; i++) {
        if (vetor[i] == '\n') {
            vetor[i] = '\0';
            break;
        }
    }
}

void sinalizaSaida(int sig){
    flag = 1;
}

void recebeMensagem(){
    char recebe[TAM_SEND] = {};
    while (1){
        int receive = recv(sockt, recebe, TAM_SEND, 0);
        if(receive > 0){
            printf("%s", recebe);
            printf("%s", "> ");
            fflush(stdout);
        }else if(receive == 0){
            break;
        }
    }
}

void enviaMensagem(){
    char mensagem[TAM_MSG] = {};
    while(1){
        while(fgets(mensagem, TAM_MSG, stdin) != NULL){
            if(strlen(mensagem) == 1){
                printf("%s", "> ");
                fflush(stdout);
            }else{
                break;
            }
        }
        if(strncmp("sair", mensagem, 4) == 0){
            break;
        }
        send(sockt, mensagem, TAM_MSG, 0);
    }
    sinalizaSaida(2);
}
