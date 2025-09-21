# Mini Chat em C com socket
Projeto acadêmico de um pequeno chat multi-cliente em C que usa socket e select com a funcão adiconal do servidor realizar operações matemáticas.

## Objetivos
- O servidor aceita múltiplas conexões usando select() e faz broadcast de mensagens.
- Cliente envia mensagens ao servidor via terminal por socket.
- O servidor aceita múltiplas conexões.
## Estrutura
- `chat_client.c`: Cliente interativo.
- `chat_server.c`: Servidor multi conexões.
- `Makefile`: Builder do projeto (server, client, run-server, run-client).
## Compilação e execução
Para compilar e executar o projeto, siga as diretivas abaixo.
### Compilar
```bash
make        # compila tudo
make server # compila o servidor
make client # compila o client
```

### Executar
Servidor
#### Com porta padrão (5050)
```bash
    ./chat_server
```
#### Com porta definida pelo usuário
```bash
    ./chat_server <porta>
```

Cliente
#### Com IP e porta padrão (0.0.0.0 5050)
```bash
    ./chat_client 
```
#### Com IP e porta definidas pelo usuário
```bash
    ./chat_client <IP> <porta>
```
