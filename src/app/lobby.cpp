	#include "lobby.h"
	
	NetPlayer* Lobby::get_player(unsigned id)
	{
		for(NetPlayer& p : player)
		{
			if(p.id == id)
			{
				return &p;
			}
		}
		return 0;
	}
	
	NetPlayer* Lobby::get_player_from_socket(int sock)
	{
		for(NetPlayer& p : player)
		{
			if(p.socket == sock)
			{
				return &p;
			}
		}
		return 0;
	}
	
	bool Lobby::all_ready() const
	{
		for(const NetPlayer& p : player)
		{
			if(!p.ready)
			{
				return false;
			}
		}
		return true;
	}
	
	void Lobby::clear()
	{
		my_id = 0;
		player.clear();
	}
