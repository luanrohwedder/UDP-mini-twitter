#ifndef CLIENT_H
#define CLIENT_H

#include "../include/message.h"
#include <unordered_map>

#define BUFFER_SIZE 1024
#define TIMEOUT_TIME 10

/**
 * @brief Implementação UDP do cliente.
 * 
 */
class Client
{
public:
    /// Contrutor Padrão
    Client() {};

    /**
     * @brief Construtor que inicializa o cliente com nome de usuário, IP e porta
     *     do servidor.
     * 
     * Inicializa o socket e o endereço do servidor.
     * 
     * @param username Nome de usuário do cliente
     * @param serverIP Endereço IP do servidor
     * @param serverPort Porta do servidor
     */
    Client(const std::string &username, const std::string&, int);

    /// Destrutor
    ~Client();

    /**
     * @brief Conecta o cliente ao servidor.
     * 
     * Envia um `Message OI` ao servidor, e espera por uma resposta em um 
     *     determinado tempo. O servidor retorna um identificador único para
     *     cliente.
     * 
     * @retval 0 - Erro
     * @retval 1 - Sucesso
     */
    int connectToServer();

    /**
     * @brief Envia uma mensagem para o destino
     * 
     * Faz o envio de `Message MSG` ao servidor, escolhendo o destino para onde
     *     será enviado.
     * 
     * @param message Conteúdo da mensagem a ser enviada
     * @param messagetType Tipo da mensagem (Padrão MSG)
     * @param destinationID ID do destino (Padrão 0)
     * 
     */
    void sendMessage(const std::string&, Message::MessageType = Message::MSG, int = 0);

    /**
     * @brief Recebe mensagens do servidor
     * 
     * @return Message* Ponteiro para a mensagem recebida
     */
    Message* receiveMessages();

    /*
    * Getters
    */
   
    int getId() const { return _id; }
    std::string getUsername() const { return _username; }
    std::unordered_map<int, ClientInfo> getClientOnline() const { return _clientsOnline; }
    bool getRunning() const { return _running; }

private:
    int _id; /** Identificador do cliente */
    std::string _username; /** Nome de usuário */    
    std::unordered_map<int, ClientInfo> _clientsOnline; /** Lista de clientes online */

    int _sockfd;  /** Descritor de socket UDP */
    struct sockaddr_in _serverAddr; /** Endereço do servidor */
    bool _running; /** Estado de execução do cliente */

    /**
     * @brief Define o tempo de timeout para o socket
     * 
     * @param timeout Tempo de timeout (Padrão 0)
     */
    void setTimeout(int = 0);

    /**
     * @brief Imrpime mensagem de erro no console
     * 
     * @param message Mensagem de erro
     */
    void error(const std::string&);
};

#endif