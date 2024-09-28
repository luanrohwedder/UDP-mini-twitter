# Mini Twitter (X) - Uma Implementação Simples do Twitter baseado em UDP

Este repositório contém o código fonte do projeto desenvolvido para a disciplina de `Redes de Computadores T01-2024-02`. O projeto apresenta uma implementação básica de uma aplicação semelhante ao Twitter, permitindo comunicação em tempo real entre usuários através de uma arquitetura cliente-servidor UDP, utilizando C++ e a biblioteca Gtkmm para a interface gráfica.

A aplicação permite que os usuários se registrem, façam login e participem de mensagens em tempo real. Este projeto serve como uma demonstração prática dos conceitos de redes, particularmente na comunicação UDP.

## Ambiente
O projeto foi desenvolvido em um ambiente `Linux`, especificamente no `Ubuntu 22.04` rodando no `WSL2`.

## Dependências
- `gtkmm 3.24.5`
- `glibmm 2.66.2`
- `sigc++ 2.10.4`

Você pode instalar essas dependências usando seu gerenciador de pacotes. Por exemplo, no Ubuntu, você pode executar:
```
sudo apt install libgtkmm-3.0-dev libglibmm-2.4-dev libsigc++-2.0-dev
```

## Compilar e Executar
Para compilar e executar o projeto, execute os seguintes comandos no diretório raiz:

### Compilar com Makefile
```
make
```

### Executar o servidor
```
./bin/server
```

### Executar o cliente
```
./bin/cliente
```

