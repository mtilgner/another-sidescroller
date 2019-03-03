#ifndef NET_H
#define NET_H

#include <vector>
#include <algorithm>

#define TCP_NODELAY
#define MAX_CLIENTS 16


namespace net
{

struct MessageHeader
{
    int type;
    size_t size;

    MessageHeader(int t = -1) : type(t), size(0){}
};

struct Message
{
    MessageHeader header;
    int from;
    std::vector<char> data;

    Message(int type = 0) : header(type), from(-1){}
};

struct Address
{
    unsigned char
	a = 0,
	b = 0,
	c = 0,
	d = 0
	;
	
    unsigned short port = 0;
};

const char* to_string(const Address& ad);
Address from_string(const char* str);

enum State
{
    DISCONNECTED = 0,
    SERVER,
    CLIENT
};

enum SystemMessage
{
    ERROR = -1,
    NONE = 0,

    CONNECTION_ACCEPTED,
    CONNECTION_LOST,

    USER_MESSAGE = 100
};

bool poll_message(Message& msg); // after receive()


namespace tcp
{

int start_server(unsigned short port);
int start_client(const char* ip_addr, unsigned short port);

int sendmsg(Message& m);
int sendmsg(Message& m, int to);

bool is_connected(int num);

int receive();
void disconnect();
void disconnect(int client);

Address getaddr(int client);
int getstate();

} // namespace tcp


template<class Pod>
inline int convmsg(const Message& m, Pod& dest)
{
    if(m.data.size() != sizeof(Pod))
        return -1;

    std::copy(m.data.begin(), m.data.end(), (char*) &dest);
    return 0;
}

template<class Pod>
inline int tomsg(const Pod& src, Message& m)
{
    const char* b = (char*) &src;
    m.data.resize(sizeof(Pod));
    m.header.size = m.data.size();
    std::copy(b, b + sizeof(Pod), m.data.begin());
    return 0;
}

class MessageReader
{
public:
	MessageReader(const Message& m) : msg_(m){}
	
	template<class Pod>
	bool read(Pod& dest)
	{
		if((int) msg_.data.size() - pos_ >= (int) sizeof(Pod))
		{
			std::copy(msg_.data.begin() + pos_, msg_.data.begin() + pos_ + sizeof(Pod), (char*) &dest);
			pos_ += sizeof(Pod);
			return true;
		}
		return false;
	}
private:
	const Message& msg_;
	int pos_ = 0;
	
};
	
class MessageWriter
{
public:
	MessageWriter(Message& m) : msg_(m){}
	
	template<class Pod>
	bool write(const Pod& src)
	{
		int pos = msg_.data.size();
		msg_.data.resize(msg_.data.size() + sizeof(Pod));
		const char* b = (char*) &src;
		std::copy(b, b + sizeof(Pod), msg_.data.begin() + pos);
		return true;
	}
private:
	Message& msg_;
};
	
} //namespace Network

#endif
