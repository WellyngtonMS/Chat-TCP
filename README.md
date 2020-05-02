# Chat-TCP
Implementação de duas aplicações em C que utiliza threads e sockets e que permite o envio de mensagens instantâneas para um conjunto de utilizadores (clientes), a partir de um servidor ao qual todos se conectam. O servido replica todas as mensagens para todos os usuários (clientes), os clientes são capazes de enviar mensagens para o servidor e exibir as mensagens recebidas. A execução do servidor é simples: deve-se solicitar a porta de execução e aguardar a conexão dos clientes. Toda nova conexão é tratada por uma thread individual, e todo evento é notificado pelo servidor (nova conexão, mensagem e fim de conexão). 

A execução do programa cliente solicita ao usuário um apelido (que servirá de prefixo para suas mensagens), o endereço IP do servidor ao qual ele se conectará e a porta utilizada. Durante sua execução o usuário poderá enviar mensagens e visualizar as mensagens enviadas pelos outros usuários. Caso o usuário digite a palavra "SAIR" o programa cliente é finalizado.
Um monitor registra em um arquivo de texto todos os eventos do servidor.

Para compilar e rodar execute primeiro o servidor, e em outra (s) janela (s) compile e execute quantos clientes quiser.
