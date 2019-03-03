	#include "client.h"
	#include "net_message.h"
	#include "application.h"
	#include "raw_data.h"
	
	#include <iostream>
	
	void Client::init()
	{
		app = &Application::get();
	}
	
	void Client::handle_msg(const net::Message& msg)
	{
		switch(msg.header.type)
		{
			case START_GAME:
				handle_start_game(msg);
				break;
			case REQUEST:
				handle_request(msg);
				break;
			case LOBBY_DATA:
				handle_lobby_data(msg);
				break;
			case TURN_FILE:
				handle_turnfile(msg);
				break;
			case BYE:
				handle_bye(msg);
				break;
			case INPUT_ALL:
				handle_input_all(msg);
				break;
		}
	}
	
	void Client::handle_start_game(const net::Message& msg)
	{
		int count;
		std::vector<RawData> rd;
		net::MessageReader mr(msg);
		mr.read(count);
		rd.resize(count);
		for(int i = 0; i < count; ++i)
		{
			RawData& raw = rd[i];
			raw.file = read_string(mr);
			int s;
			mr.read(s);
			raw.data.resize(s);
			for(int j = 0; j < s; ++j)
			{
				mr.read(raw.data[s]);
			}
		}
		
		for(const RawData& r : rd)
		{
			save_raw_data(r);
		}
		
		app->client_start_game();
	}
	
	void Client::handle_request(const net::Message& msg)
	{
		
	}
	
	void Client::handle_lobby_data(const net::Message& msg)
	{
		net::MessageReader rd(msg);
		
		rd.read(app->lobby.my_id);
		size_t s = 0;
		rd.read(s);
		app->lobby.player.clear();
		app->lobby.player.resize(s);
		for(NetPlayer& p : app->lobby.player)
		{
			rd.read(p.id);
			p.name = read_string(rd);
			rd.read(p.ready);
			rd.read(p.socket);
		}
		
		if(app->state == LOBBY)
		{
			app->ui_lobby.update();
		}
		else if(app->state == GAME)
		{
			// update something else
		}
	}
	
	void Client::handle_turnfile(const net::Message& msg)
	{
		
	}
	
	void Client::handle_bye(const net::Message& msg)
	{
		app->return_to_main();
	}
	
	
	void Client::handle_input_all(const net::Message& msg)
	{
		int count, player_id;
		CharacterInput in;
		Game& g = app->game;
		
		net::MessageReader mr(msg);
		
		mr.read(count);
		
		for(int i = 0; i < count; ++i)
		{
			mr.read(player_id);
			mr.read(in);
			g.current_inputs[player_id] = in;
		}
	}
			
			
	
	
	
	void Client::send_hello()
	{
		net::Message msg(HELLO);
		net::MessageWriter wr(msg);
		write_string(wr, app->settings.player_name);
		
		net::tcp::sendmsg(msg);
	}
	
	void Client::send_ready()
	{
		net::Message msg(READY);
		net::tcp::sendmsg(msg);
	}
	
	void Client::send_client_file()
	{
		
	}
	
	void Client::send_bye()
	{
		net::Message msg(BYE);
		net::tcp::sendmsg(msg);
	}
	
	void Client::send_input_single()
	{
		int player_id = app->lobby.my_id;
		Game& g = app->game;
		
		net::Message msg(INPUT_SINGLE);
		net::MessageWriter mw(msg);
		
		mw.write(player_id);
		mw.write(g.next_input);
		
		net::tcp::sendmsg(msg);
	}
	
	
				
				
			
