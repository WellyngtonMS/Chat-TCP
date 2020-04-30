#define TAM_NAME 31
#define TAM_MSG 101
#define TAM_SEND 201

typedef struct Cliente{
    int data;
    struct Cliente* prev;
    struct Cliente* link;
    char ip[16];
    int complemento;
    char nome[31];
}TCliente;

typedef struct monitor{
	char caminho[20];
	char informacao[120];
	int linhas;
}TMonitor;

int servidor_sockt = 0, cliente_sockt = 0, porta;
TCliente *inicio, *atual;
TMonitor monitor;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_t threadMonitor;

TCliente *novo(int sockfd, char* ip, int complemento){
    TCliente *np = (TCliente *)malloc(sizeof(TCliente));
    np->data = sockfd;
    np->prev = NULL;
    np->link = NULL;
    strncpy(np->ip, ip, 16);
    np->complemento = complemento;
    strncpy(np->nome, "NULL", 5);
    return np;
}

void inicializarMonitor(TMonitor* mon){
	char fileName[30];
	sprintf(fileName, "servidor %d.txt", porta);
   	FILE *file_descriptor;
  	file_descriptor = fopen(fileName, "a");
  	if (file_descriptor == NULL){
        printf("Problemas na criacao do arquivo!\n");
        return;
    }
	fclose(file_descriptor);
	mon->linhas = 0;
	strcpy(mon->caminho, fileName);
}

void inserirLinha(TMonitor* mon){
    int result = 0;

	pthread_mutex_lock(&mutex1);
	FILE *file_descriptor;
	file_descriptor = fopen(mon->caminho, "a");
	result = fprintf(file_descriptor,"%s", mon->informacao);
	if(result == EOF){
        printf("Erro na gravacao\n");
    }
    fclose(file_descriptor);
    mon->linhas++;
	pthread_mutex_unlock(&mutex1);
}

void encerraServidor(int sig){
    TCliente *aux;
    while (inicio != NULL){
        close(inicio->data);
        aux = inicio;
        inicio = inicio->link;
        free(aux);
    }
    printf("\nServidor offline!\n");
    printf("Verifique o arquivo txt para obter as informacoes gravadas!\n");
    exit(EXIT_SUCCESS);
}

void enviaTodosClientes(TCliente *np, char tmp_buffer[]){
    TCliente *tmp = inicio->link;
    if(strlen(tmp_buffer) == 0){
        exit;
    }
    else{
        printf("Mensagem de %s:%d %s", np->ip, np->complemento, tmp_buffer);
        sprintf(monitor.informacao, "Mensagem de %s:%d %s", np->ip, np->complemento, tmp_buffer);
        if(pthread_create(&threadMonitor, NULL, (void*)inserirLinha, (void*)&monitor) != 0){
            printf ("Erro ao criar thread!\n");
            exit(EXIT_FAILURE);
        }
        while(tmp != NULL){
            if(np->data != tmp->data){
                send(tmp->data, tmp_buffer, TAM_SEND, 0);
                tmp = tmp->link;
            }
            else{
                send(tmp->data, tmp_buffer, TAM_SEND, 0);
                tmp = tmp->link;
            }
        }
    }
}

void manipulaCliente(void *p_client){
    int flag = 0;
    char nome[TAM_NAME] = {};
    char recebe_buffer[TAM_MSG] = {};
    char envia_buffer[TAM_SEND] = {};
    TCliente *np = (TCliente *)p_client;

    if(recv(np->data, nome, TAM_NAME, 0) <= 0 || strlen(nome) < 2 || strlen(nome) >= TAM_NAME-1){
        printf("%s não inseriu nome.\n", np->ip);
        flag = 1;
    } else{
        strncpy(np->nome, nome, TAM_NAME);
    }

    while(1){
        if(flag){
            break;
        }
        int receive = recv(np->data, recebe_buffer, TAM_MSG, 0);
        if(receive > 0){
            if(strlen(recebe_buffer) == 1){
                continue;
            }
            sprintf(envia_buffer, "%s: %s", np->nome, recebe_buffer);
        }else if(receive == 0){
            memset(&envia_buffer, 0, sizeof (envia_buffer));
            memset(&recebe_buffer, 0, sizeof (recebe_buffer));
            printf("Cliente com o IP %s:%d se desconectou.\n", np->ip, np->complemento);
            sprintf(monitor.informacao, "Cliente com o IP %s:%d se desconectou.\n", np->ip, np->complemento);
            if(pthread_create(&threadMonitor, NULL, (void*)inserirLinha, (void*)&monitor) != 0){
                printf ("Erro ao criar thread!\n");
                exit(EXIT_FAILURE);
            }
            flag = 1;
            break;
        }else{
            printf("Fatal Error: -1\n");
            flag = 1;
            break;
        }
        enviaTodosClientes(np, envia_buffer);
        memset(&envia_buffer, 0, sizeof (envia_buffer));
    }

    close(np->data);
    if (np == atual){
        atual = np->prev;
        atual->link = NULL;
    } else{
        np->prev->link = np->link;
        np->link->prev = np->prev;
    }
    free(np);
}
