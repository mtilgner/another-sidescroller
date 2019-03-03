	#ifndef LOBBY_H
	#define LOBBY_H
	
	#include <string>
	#include <vector>
	
	struct NetPlayer
	{
		unsigned id = 0;
		std::string name;
		bool ready = false;
		int socket = -1;
	};
	
	struct Lobby
	{
		unsigned my_id = 0;
		std::vector<NetPlayer> player;
		NetPlayer* get_player(unsigned id);
		NetPlayer* get_player_from_socket(int sock);
		bool all_ready() const;
		void clear();
	};
	
	
	
	
	#endif
