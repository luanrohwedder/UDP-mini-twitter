#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <cstring>
#include <arpa/inet.h>

/**
 * @brief Estrutura auxiliar para armazenar informações de um cliente.
 * 
 * Armazena o endereço do cliente e o nome de usuário.
 */
struct ClientInfo {
    sockaddr_in address;
    std::string username;
};

/**
 * @brief Implementação do protocolo de mensagem.
 * 
 * A classe `Message` representa uma mensagem trocada entre cliente e servidor
 *     no sistema de comunicação UDP. Ela inclui funcionalidades para enviar, 
 *     receber e manipular mensagens de diferentes tipos.
 */
class Message
{
public:
    /**
     * @brief Tipos de mensagens suportados pelo protocolo
     * 
     * Enumeração que define os diferentes tipos de mensagem trocados entre
     * o cliente e o servidor.
     */
    enum MessageType
    {
        OI = 0,     /** Mensagem de conexão */
        TCHAU = 1,  /** Mensagem de desconexão */
        MSG = 2,    /** Mensagem de texto */
        ERRO = 3,   /** Mensagem de erro */
        LIST = 4    /** Mensagem solicitando lista de clientes */
    };

    /**
     * @brief Construtor padrão da classe Message
     * 
     * Inicializa uma mensagem com todos os campos zerados.
     */
    Message() : _type(0), _originID(0), _destinationID(0), _textSize(0)
    {
        memset(_username, 0, sizeof(_username));
        memset(_text, 0, sizeof(_text));
    }

    /**
     * @brief Construtor da classe Message
     * 
     * @param type Tipo da mensagem
     * @param origin ID de origem da mensagem
     * @param destination ID de destino da mensagem
     * @param username Nome de usuário que está enviando a mensagem
     * @param text Texto da mensagem
     */
    Message(int type, int origin, int destination, const std::string &username, const std::string &text)
        : _type(type), _originID(origin), _destinationID(destination)
    {
        setUsername(username);
        setText(text);
    }

    /**
     * @brief Envia a mensagem por meio de um socket
     * 
     * @param sockfd Descritor de socket para envio
     * @param addr Endereço do destinatário
     */
    inline void send(int sockfd, struct sockaddr_in addr)
    {
        this->networkByteOrder();
        sendto(sockfd, this->data(), this->size(), 0, (struct sockaddr*)&addr, sizeof(addr));
    }

    /**
     * @brief Recebe uma mensagem de um socket
     * 
     * @param sockfd Descritor de socket de onde a mensagem será recebida
     * @param addr Endereço do remetente da mensagem
     * @param buffer_size Tamanho do buffer para receber a mensagem
     * 
     * @return Message* Ponteiro para a mensagem recebida
     */
    static Message* receive(int sockfd, struct sockaddr_in &addr, int buffer_size)
    {
        char buffer[buffer_size];
        socklen_t addrLen = sizeof(addr);

        int n = recvfrom(sockfd, buffer, buffer_size, 0,(struct sockaddr *)&addr, &addrLen);

        if (n > 0)
        {
            buffer[n] = '\0';
            Message* msg = new Message();
            memcpy(msg, buffer, sizeof(Message));
            msg->hostByteOrder();

            return msg;
        }
        else
        {
            return nullptr;
        }
    }

    /*
    * Getters
    */

    int getType() const { return _type; }
    int getOriginID() const { return _originID; }
    int getDestinationID() const { return _destinationID; }
    int getTextSize() const { return _textSize; }
    std::string getUsername() const { return std::string(_username); }
    std::string getText() const { return std::string(_text); }

private:
    int _type;             /** Tipo da mensagem */
    int _originID;         /** ID de origem da mensagem */
    int _destinationID;    /** ID de destino da mensagem */
    int _textSize;         /** Tamanho do texto da mensagem */
    char _username[21];    /** Nome de usuário (limite de 20 caracteres) */
    char _text[141];       /** Texto da mensagem (limite de 140 caracteres) */

    /**
     * @brief Define o texto da mensagem
     * 
     * @param msg Texto da mensagem
     */
    inline void setText(const std::string &msg)
    {
        _textSize = msg.size();
        strncpy(_text, msg.c_str(), sizeof(_text) - 1);
        _text[_textSize] = '\0';
    }

    /**
     * @brief Define o nome de usuário na mensagem
     * 
     * @param name Nome de usuário
     */
    inline void setUsername(const std::string &name)
    {
        stpncpy(_username, name.c_str(), sizeof(_username) - 1);
        _username[sizeof(_username) - 1] = '\0';
    }

    /**
     * @brief Converte os valores internos para ordem de bytes de rede
     * 
     * Converte os campos inteiros da mensagem para a ordem de bytes usada
     * em redes (big-endian).
     */
    inline void networkByteOrder()
    {
        _type = htonl(_type);
        _originID = htonl(_originID);
        _destinationID = htonl(_destinationID);
        _textSize = htonl(_textSize);
    }

    /**
     * @brief Converte os valores internos para ordem de bytes do host
     * 
     * Converte os campos inteiros da mensagem para a ordem de bytes usada
     * pela máquina local (host).
     */
    inline void hostByteOrder()
    {
        _type = ntohl(_type);
        _originID = ntohl(_originID);
        _destinationID = ntohl(_destinationID);
        _textSize = ntohl(_textSize);
    }

    /**
     * @brief Obtém um ponteiro para os dados da mensagem
     * 
     * @return const char* Ponteiro para os dados da mensagem
     */
    const char *data() const { return reinterpret_cast<const char *>(this); }

    /**
     * @brief Obtém o tamanho da mensagem em bytes
     * 
     * @return size_t Tamanho da mensagem
     */
    size_t size() const { return sizeof(Message); }
};

#endif