#ifndef SERVER_H
#define SERVER_H

#include "../include/message.h"
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <csignal>
#include <fstream>

#define BUFFER_SIZE 1024
#define TIMER 60

/**
 * @brief Implementação do servidor UDP.
 * 
 * A classe `Server` representa o servidor que gerencia conexões de clientes, 
 * escuta mensagens e responde a pedidos no sistema de comunicação UDP.
 * 
 */
class Server
{
public:
    /**
     * @brief Construtor da classe Server.
     * 
     * Inicializa o servidor UDP com a porta especificada, bind do socket e 
     *     inicío do UpTime.
     * 
     * @param ip IP do servidor
     * @param port Porta em que o servidor vai escutar.
     * 
     */
    Server(const std::string&, int);

    /**
     * @brief Destrutor da classe Server.
     * 
     * Libera recursos alocados pelo servidor.
     * 
     */
    ~Server();

    /**
     * @brief Inicia o servidor.
     * 
     * Começa a escutar e processar as mensagens dos clientes e envio de mensagem
     *     de status com sinal.
     * 
     */
    void start();

    /**
     * @brief Envia o status do servidor para todos os clientes conectados.
     * 
     * A cada intervalo de tempo, operado por um sinal, envia informações sobre 
     *     o estado atual do servidor.
     * 
     */
    void sendServerStatus();

private:
    int _sockfd;                                      /** Descritor de socket do servidor */
    int _idCount;                                     /** Contador de IDs para os clientes */
    bool _running;                                    /** Flag para indicar se o servidor está rodando */
    const std::string _serverID = "UDP_SERVER";       /** Identificador do servidor */
    struct sockaddr_in _serverAddr;                   /** Endereço do servidor */
    std::ofstream _file;                              /** Arquivo de log para o servidor */
    std::unordered_map<int, ClientInfo> _clients;     /** Mapeamento de clientes conectados */
    std::mutex _clientsMutex;                         /** Mutex para proteger acesso à lista de clientes */
    std::chrono::time_point<std::chrono::steady_clock> startTime; /** Momento de início do servidor */

    /**
     * @brief Função para ouvir mensagens dos clientes.
     * 
     * Por um thread separada, escuta ativamente por mensagens e as distribui 
     *     para o tratamento adequado.
     * 
     */
    void listen();

    /**
     * @brief Configura o timer do servidor.
     * 
     * Define o intervalo de tempo para envio de sinais.
     * 
     */
    void setupTimer();
    
    /**
     * @brief Lida com mensagens recebidas de um cliente.
     * 
     * Processa as mensagens enviadas pelos clientes e realiza a ação necessária.
     * 
     * @param msg Ponteiro para a mensagem recebida.
     * 
     */
    void handleClient(Message*);

    /**
     * @brief Lida com pedidos de lista de clientes online.
     * 
     * Responde a um cliente com a lista de todos os clientes conectados ao servidor.
     * 
     * @param clientAddr Endereço do cliente que fez o pedido.
     * @param msg Ponteiro para a mensagem recebida.
     * 
     */
    void handleClientListRequest(struct sockaddr_in, Message*);

    /**
     * @brief Envia uma mensagem para todos os clientes conectados.
     * 
     * Realiza o broadcast de uma mensagem para todos os clientes. conectados ao
     *     servidor.
     * @param msg Ponteiro para a mensagem a ser enviada.
     * 
     */
    void broadcastMessage(Message*);

    /**
     * @brief Envia uma mensagem privada para um cliente.
     * 
     * Faz o envio de uma mensagem privada para o cliente selecionado. Caso não
     *     exista o cliente, retorna uma mensagem de erro.
     * 
     * @param msg Ponteiro para a mensagem a ser enviada.
     * 
     */
    void privateMessage(Message*);
    
    /**
     * @brief Adciona cliente ao servidor.
     * 
     * Registra um cliente ao servidor quando ele se conecta pela primeira vez.
     * 
     * @param clientAddr Endereço do cliente.
     * @param msg Ponteiro para a mensagem de conexão.
     * 
     */
    void addClient(struct sockaddr_in, Message*);

    /**
     * @brief Remove cliente do servidor.
     * 
     * Desconecta e remove um cliente do servidor.
     * 
     * @param clientAddr Endereço do cliente.
     * @param msg Ponteiro para a mensagem de desconexão.
     * 
     */
    void deleteClient(struct sockaddr_in, Message*);

    /**
     * @brief Verifica se um cliente existe.
     * 
     * Checa se um cliente com determinado ID já está conectado ao servidor.
     * 
     * @param clientId ID do cliente a ser verificado.
     * 
     * @retval `true` Se o cliente existe.
     * @retval `false` Se o cliente não existe.
     * 
     */
    bool clientExists(int);

    /**
     * @brief Obtém o tempo decorrido desde o início do servidor.
     * 
     * @return std::string Tempo decorrido em formato legível.
     */
    std::string getElapsedTime();

    /**
     * @brief Obtém o horário atual.
     * 
     * @return std::string Horário atual em formato legível.
     */
    std::string getCurrentTime();

    /**
     * @brief Log do servidor
     * 
     * Faz o log do servidor, imprimindo no console e escrevendo em um arquivo
     *     de log.
     * 
     * @param msg Ponteiro para a mensagem recebida.
     * @param clientAddr Endereço do cliente.
     * @param isAdd Indica se o log é para adicionar um novo cliente (true) 
     *     ou remover (false).
     */
    void log(Message*, struct sockaddr_in, bool);

    /**
     * @brief Imrpime mensagem de erro no console.
     * 
     * @param message Mensagem de erro.
     */
    void error(const std::string&);
};

#endif