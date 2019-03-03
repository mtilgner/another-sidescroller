	#include "host.h"
	#include "net_message.h"
	#include "application.h"
	#include "texture.h"
	#include "raw_data.h"
	
	#include <iostream>
	
	void Host::init()
	{
		app = &Application::get();
	}
	
	void Host::handle_msg(const net::Message& msg)
	{
		switch(msg.header.type)
		{
			case HELLO:
				handle_hello(msg);
				break;
			case READY:
				handle_ready(msg);
				break;
			case CLIENT_FILE:
				handle_client_file(msg);
				break;
			case BYE:
				handle_bye(msg);
				break;
			case INPUT_SINGLE:
				handle_input_single(msg);
				break;
		}
	}

	void Host::handle_hello(const net::Message& msg)
	{
		net::MessageReader rd(msg);
		
		unsigned id = 0;
		
		for(unsigned i = 2; i < MAX_CLIENTS; ++i)
		{
			if(app->lobby.get_player(i) == 0)
			{
				id = i;
				break;
			}
		}
		
		NetPlayer p;
		p.id = id;
		p.name = read_string(rd);
		p.socket = msg.from;
		
		app->lobby.player.push_back(p);
		app->ui_lobby.update();
		
		send_lobby_data();
	}
	
	void Host::handle_ready(const net::Message& msg)
	{
		NetPlayer* p = app->lobby.get_player_from_socket(msg.from);
		p->ready = true;
		
		if(app->lobby.all_ready())
		{
			if(app->state == LOBBY)
			{
				app->host_start_game();
			}
			else if(app->state == GAME)
			{
				// new turn
			}
		}
		else
		{
			send_lobby_data();
			if(app->state == LOBBY)
			{
				app->ui_lobby.update();
			}
			else if(app->state == GAME)
			{
				// update something else
			}
		}
	}
		
	
	void Host::handle_client_file(const net::Message& msg)
	{
		
	}
	
	void Host::handle_bye(const net::Message& msg)
	{
		int sock = msg.from;
		net::tcp::disconnect(sock);
		Lobby& l = app->lobby;
		std::vector<NetPlayer> vp;
		for(NetPlayer p : l.player)
		{
			if(p.socket != sock)
			{
				vp.push_back(p);
			}
		}
		l.player = vp;
		send_lobby_data();
		if(app->state == LOBBY)
		{
			app->ui_lobby.update();
		}
		else if(app->state == GAME)
		{
			//probably end game when player leaves
		}
	}
	
	void Host::handle_input_single(const net::Message& msg)
	{
		net::MessageReader mr(msg);
		int player_id; CharacterInput in;
		mr.read(player_id);
		mr.read(in);
		
		Game& g = app->game;
		
		//std::cout << "statechange_single, player_id = " << player_id << ", tick = " << cs.tick << ", current_tick = " << g.current_tick << "\n";
		
		if(in.tick == g.current_tick)
		{
			g.current_inputs[player_id] = in;
			if(g.players_ready())
			{
				send_input_all();
			}
		}
		else if(in.tick == g.current_tick + 1)
		{
			g.new_inputs[player_id] = in;
		}	
	}
	
	void Host::send_start_game()
	{
		
		net::Message msg(START_GAME);
		
		std::vector<RawData> rd;
		
		rd.push_back(load_raw_data("props.txt"));
		rd.push_back(load_raw_data("map.txt"));
		
		auto L = TextureManager::get().loaded_texture_names();
		
		for(std::string str : L)
		{
			rd.push_back(load_raw_data(str + ".png"));
		}
		
		net::MessageWriter mw(msg);
		int count = rd.size();
		
		mw.write(count);
		for(const RawData& raw : rd)
		{
			write_string(mw, raw.file);
			int s = raw.data.size();
			mw.write(s);
			for(char c : raw.data)
			{
				mw.write(c);
			}
		}
		
		
		net::tcp::sendmsg(msg);
	}
	
	void Host::send_request()
	{
		
	}
	
	void Host::send_lobby_data()
	{
		for(const NetPlayer& p : app->lobby.player)
		{
			if(p.id != app->lobby.my_id)
			{
				net::Message msg(LOBBY_DATA);
				net::MessageWriter wr(msg);
				
				wr.write(p.id);
				wr.write(app->lobby.player.size());
				for(const NetPlayer& it : app->lobby.player)
				{
					wr.write(it.id);
					write_string(wr, it.name);
					wr.write(it.ready);
					wr.write(it.socket);
				}
				
				net::tcp::sendmsg(msg, p.socket);
			}
		}
	}
		
	
	void Host::send_turnfile()
	{
		
	}
	
	void Host::send_bye()
	{
		net::Message msg(BYE);
		net::tcp::sendmsg(msg);
	}
	
	
	void Host::send_input_all()
	{
		net::Message msg(INPUT_ALL);
		net::MessageWriter mw(msg);
		Game& g = app->game;
		
		mw.write((int)g.current_inputs.size());
		for(auto& p : g.current_inputs)
		{
			mw.write((int) p.first);
			mw.write(p.second);
			
			//std::cout << "sending statechange_all, player_id = " << p.first << ", tick = " << p.second.tick << "\n";
		}
		
		net::tcp::sendmsg(msg);
	}
