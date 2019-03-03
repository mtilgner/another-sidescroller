	#ifndef NET_MESSAGE_H
	#define NET_MESSAGE_H
	
	#include "Net.h"
	#include <string>
	
	enum MSG_TYPE
	{
		HELLO = net::USER_MESSAGE,
		BYE,
		READY,
		LOBBY_DATA,
		START_GAME,
		CLIENT_FILE,
		TURN_FILE,
		REQUEST,
		INPUT_SINGLE,
		INPUT_ALL
	};
	
	std::string read_string(net::MessageReader& rd);
	void write_string(net::MessageWriter& wr, std::string str);
	
	
	
	
	#endif
