#include <memory>
#include <list>
#include <map>
#include <string.h>
#include <assert.h>

#include <SDL2/SDL_net.h>
//#include <SDL2/SDL.h>

#ifndef MATE_COMPILE
#include "Net.h"
#endif

namespace net
{

static bool was_init = false;

int netinit();
void netquit();

class SocketHolder
{
    static SDLNet_SocketSet sockset_;
    static size_t maxsockets_;
    TCPsocket tcpsocket_;
public:
    SocketHolder(TCPsocket sock) : tcpsocket_(sock)
    {
        if(sockset_)
        {
            SDLNet_TCP_AddSocket(sockset_, tcpsocket_);
        }
        else
        {
            tcpsocket_ = 0;
        }
    }

    ~SocketHolder()
    {
        if(tcpsocket_)
        {
            if(sockset_)
            {
                SDLNet_TCP_DelSocket(sockset_, tcpsocket_);
            }
            SDLNet_TCP_Close(tcpsocket_);
            tcpsocket_ = 0;
        }
    }
    TCPsocket tcp_socket() const
    {
        return tcpsocket_;
    }

    static int check_sockets(unsigned timeout = 0)
    {
        if(sockset_)
            return SDLNet_CheckSockets(sockset_, timeout);

        return 0;
    }

    static size_t maxsockets()
    {
        return maxsockets_;
    }

    static void init(size_t max)
    {
        maxsockets_ = max;
        if(sockset_)
        {
            SDLNet_FreeSocketSet(sockset_);
            sockset_ = 0;
        }
        sockset_ = SDLNet_AllocSocketSet(maxsockets_);
    }

    static void quit()
    {
        SDLNet_FreeSocketSet(sockset_);
    }
};

SDLNet_SocketSet SocketHolder::sockset_ = 0;
size_t SocketHolder::maxsockets_ = 0;


void swap_msg(Message& src, Message& dst)
{
    dst.from = src.from;
    dst.header = src.header;
    dst.data.swap(src.data);
}

typedef std::list<Message> MessageList;
static MessageList msglist_;


bool poll_message(Message& msg)
{
    if(!msglist_.empty())
    {
        swap_msg(msglist_.front(), msg);
        msglist_.pop_front();
        return true;
    }
    return false;
}

Address conv_addr(IPaddress* ia)
{
    Address ad;
    int ip = SDLNet_Read32(&ia->host);

    ad.a = (ip & 0xff000000) >> 24;
    ad.b = (ip & 0xff0000) >> 16;
    ad.c = (ip & 0xff00) >> 8;
    ad.d = (ip & 0xff);
    ad.port = SDLNet_Read16(&ia->port);

    return ad;
}

void conv_addr(const Address& ad, IPaddress* ia)
{
    int ip = ad.a;
    ip = (ip << 8) | ad.b;
    ip = (ip << 8) | ad.c;
    ip = (ip << 8) | ad.d;

    SDLNet_Write32(ip, &ia->host);
    SDLNet_Write16(ad.port, &ia->port);

}

const char* to_string(const Address& ad)
{
    static char buf[256];
    sprintf(buf, "%d.%d.%d.%d:%d",
            ad.a, ad.b, ad.c, ad.d, ad.port);

    return buf;
}

Address from_string(const char* str)
{
    static char buf[256];
    static char delim[] = ".:";

    char* subs = 0;
    Address ad;

    strcpy(buf, str);

    subs = strtok(buf, delim);
    if(subs) ad.a = atoi(subs);

    subs = strtok(0, delim);
    if(subs) ad.b = atoi(subs);

    subs = strtok(0, delim);
    if(subs) ad.c = atoi(subs);

    subs = strtok(0, delim);
    if(subs) ad.d = atoi(subs);

    subs = strtok(0, delim);
    if(subs) ad.port = atoi(subs);

    return ad;
}

// ****************************** tcp ******************************
namespace tcp
{

static State state_ = DISCONNECTED;


int getstate()
{
    return state_;
}

// ******************** tcp::MessageBuf ********************

class MessageBuf
{
    Message msg_;
    char headbuf_[sizeof(MessageHeader)];
    unsigned
    headerbytes_,
    msgbytes_;

    void sys_msg(int m)
    {
        msg_ = Message(m);
        headerbytes_ = sizeof(MessageHeader);
        msgbytes_ = 0;
    }

public:

    MessageBuf() : msg_(0), headerbytes_(0), msgbytes_(0)
    {
        for(char& c : headbuf_) c = 0;
    }

    bool done() const
    {
        return sizeof(MessageHeader) == headerbytes_ && msg_.header.size == msgbytes_;
    }

    bool poll_msg(Message& dst)
    {
        if(!done()) return false;

        swap_msg(msg_, dst);
        msg_ = Message();
        headerbytes_ = msgbytes_ = 0;

        return true;
    }

    const Message& message() const
    {
        return msg_;
    }

    bool receive(TCPsocket sock)
    {
        if(!done())
        {
            int r = 0;

            if(headerbytes_ < sizeof(MessageHeader))
            {
                r = SDLNet_TCP_Recv(sock, headbuf_ + headerbytes_, sizeof(MessageHeader) - headerbytes_);
                if(r < 0)
                {
                    sys_msg(ERROR);
                }
                else
                {
                    headerbytes_ += r;
                    if(sizeof(MessageHeader) == headerbytes_)
                    {
                        memcpy(&msg_.header, headbuf_, sizeof(msg_.header));
                        msg_.data.resize(msg_.header.size);
                    }
                }
            }

            if(msgbytes_ < msg_.header.size)
            {
                r = SDLNet_TCP_Recv(sock, &msg_.data[msgbytes_], msg_.header.size - msgbytes_);
                if(r < 0)
                {
                    sys_msg(ERROR);
                }
                else
                {
                    msgbytes_ += r;
                }
            }

        }

        return done();
    }
};

// ******************** tcp::Socket ********************

class Socket
{
    std::unique_ptr<SocketHolder> psock_;
    MessageBuf msgbuf_;
public:

    bool closed() const
    {
        return psock_.get() ? false : true;
    }

    void close()
    {
        msgbuf_ = MessageBuf();
        psock_.reset(0);
    }

    void accept_connection(const Socket& server)
    {
        close();

        if(server.closed()) return;

        TCPsocket remote = 0;

        remote = SDLNet_TCP_Accept(server.psock_->tcp_socket());
        if(remote)
        {
            psock_.reset(new SocketHolder(remote));
        }

    }

    void start_server(Uint16 port)
    {
        close();
        IPaddress ip;
        ip.host = INADDR_ANY;
        SDLNet_Write16(port, &ip.port);

        TCPsocket local = SDLNet_TCP_Open(&ip);
        if(local)
        {
            psock_.reset(new SocketHolder(local));
        }

    }

    void connect(const char* ip_addr, Uint16 port)
    {
        close();
        IPaddress ip;
        Address ad = from_string(ip_addr);
        ad.port = port;

        conv_addr(ad, &ip);

        TCPsocket local = SDLNet_TCP_Open(&ip);
        if(local)
        {
            psock_.reset(new SocketHolder(local));
        }
    }

    bool has_activity() const
    {
        if(closed()) return false;

        return SDLNet_SocketReady(psock_->tcp_socket()) ? true : false;
    }

    Address get_address() const
    {
        static Address ad;
        if(!closed())
        {
            IPaddress* ia = SDLNet_TCP_GetPeerAddress(psock_->tcp_socket());

            if(ia)
            {
                 return conv_addr(ia);
            }
        }
        return ad;
    }

    bool receive()
    {
        if(closed()) return false;
        return msgbuf_.receive(psock_->tcp_socket());
    }

    bool send(Message& m)
    {
        if(closed()) return false;

        TCPsocket socket = psock_->tcp_socket();
        m.header.size = m.data.size();

        int result = SDLNet_TCP_Send(socket, &m.header, sizeof(MessageHeader));
        if(result < (int) sizeof(MessageHeader))
            return false;

        if(!m.data.empty())
        {
            result = SDLNet_TCP_Send(socket, (void*) &m.data.front(), m.data.size());
            if(result < (int) m.data.size())
                return false;
        }
        return true;
    }

    bool poll_msg(Message& dst)
    {
        if(msgbuf_.poll_msg(dst))
        {
            if(dst.header.type < 0)
            {
                close();
            }
            return true;
        }
        return false;
    }
};

// ******************************

typedef std::vector<Socket> SocketList;

static Socket local_, server_;
static SocketList socketlist_;

// ****************************** tcp functions


void disconnect()
{
    local_.close();
    server_.close();
    for(Socket& s : socketlist_) s.close();
    state_ = DISCONNECTED;
}
	
void disconnect(int client)
{
	if(client >= 0 && client < (int) socketlist_.size())
	{
		socketlist_[client].close();
	}
}
	

int net_abort()
{
    disconnect();
    return -1;
}

int start_server(Uint16 port)
{
    if(-1 == netinit()) return ERROR;

    disconnect();
    server_.start_server(port);
    if(server_.closed()) return ERROR;

    state_ = SERVER;
    return 0;
}


int start_client(const char* ip_addr, Uint16 port)
{
    if(-1 == netinit()) return ERROR;

    disconnect();
    local_.connect(ip_addr, port);

    if(local_.closed()) return ERROR;

    state_ = CLIENT;
    return 0;
}

void poll_socket(Socket& sock, int num)
{
    if(sock.has_activity())
    {
        if(sock.receive())
        {
            Message msg;
            if(sock.poll_msg(msg))
            {
                msg.from = num;
                msglist_.push_back(msg);
            }
        }
    }
}

int receive()
{
    SocketHolder::check_sockets();

    if(SERVER == state_)
    {
        if(server_.has_activity())
        {
            int i = 0;
            for(Socket& s : socketlist_)
            {
                if(s.closed())
                {
                    s.accept_connection(server_);
                    if(!s.closed())
                    {
                        Message m(CONNECTION_ACCEPTED);
                        m.from = i;
                        msglist_.push_back(m);
                    }
                    break;
                }
                ++i;
            }
        }
        int i = 0;
        for(Socket& s : socketlist_)
        {
            poll_socket(s, i);
            ++i;
        }
    }
    else if (CLIENT == state_)
    {
        poll_socket(local_, 0);
    }
    return msglist_.size();
}

Address getaddr(int client)
{
    if(SERVER == state_)
    {
        return socketlist_[client].get_address();
    }
    else if(CLIENT == state_)
    {
        return local_.get_address();
    }
    static Address ad;
    return ad;
}

int sendmsg(Message& m, int to)
{
    if(-1 == netinit()) return ERROR;

    if(SERVER == state_)
    {
        if(to >= 0 && to < (int) socketlist_.size())
        {
            Socket& s = socketlist_[to];
            if(!s.closed())
            {
                if(s.send(m))
                {
                    return 0;
                }
                else
                {
                    Message mlost(CONNECTION_LOST);
                    mlost.from = to;
                    msglist_.push_back(mlost);
                    s.close();
                }
            }
        }
    }
    else if(CLIENT == state_ && 0 == to)
    {
        if(!local_.closed())
        {
            if(local_.send(m)) return 0;
        }

    }
    return ERROR;
}

int sendmsg(Message& m)
{
    if(-1 == netinit()) return ERROR;

    if(SERVER == state_)
    {
        for(int i = 0; i < MAX_CLIENTS; ++i)
        {
            Socket& s = socketlist_[i];
            if(!s.closed())
            {
                if(!s.send(m))
                {
                    Message mlost(CONNECTION_LOST);
                    mlost.from = i;
                    msglist_.push_back(mlost);
                    s.close();
                }
            }
        }
        return 0;
    }
    else if(CLIENT == state_)
    {
        if(!local_.closed())
        {
            if(local_.send(m))
            {
                return 0;
            }
            else
            {
                Message mlost(CONNECTION_LOST);
                mlost.from = 0;
                msglist_.push_back(mlost);
                local_.close();
            }
        }
    }

    return ERROR;
}

bool is_connected(int num)
{
    if(SERVER == state_)
    {
        if(num >= 0 && num < (int) socketlist_.size())
        {
            return !socketlist_[num].closed();
        }
        return false;
    }
    else if(CLIENT == state_ && 0 == num)
    {
        return true;
    }
    return false;

}



} // namespace tcp


int netinit()
{
    if(!was_init)
    {
        if(-1 == SDLNet_Init())
        {
            return -1;
        }
        SocketHolder::init(MAX_CLIENTS + 3);
        tcp::socketlist_.resize(MAX_CLIENTS);
        atexit(netquit);
        was_init = true;
    }
    return 0;
}


void netquit()

{
//    disconnect();
    tcp::local_.close();

    tcp::server_.close();
    for(tcp::Socket& s : tcp::socketlist_)
        s.close();


    SocketHolder::quit();
    SDLNet_Quit();
    was_init = false;
}




} // namespace network
