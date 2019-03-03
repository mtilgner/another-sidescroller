	#include "ui_lobby.h"
	#include "application.h"
	
	void UILobby::init()
	{
		using namespace tab;
		
		app = &Application::get();
		
		TableAttributes ta;
		SDL_Rect rect = {50, 50, 300, 75};
		ta.rect = rect;
		ta.visible_rows = 3;
		ta.cell_border = 5;
		ta.table_border = 5;
		
		player_table.set_attributes(ta);
		player_table.set_num_columns(2);
		player_table.show(false);
		
		ta.rect.y += ta.rect.h;
		ta.rect.h = 25;
		ta.visible_rows = 1;
		
		menu.set_attributes(ta);
		menu.show(false);
	}
	
	void UILobby::load_ui()
	{
		using namespace tab;
		
		menu.clear_tab(0);
		
		Row r = menu.add_row();
		make_button(r[0]);
		r[0] << "Ready";
		r[0].handle = std::bind(&UILobby::cb_ready, this);
		
		menu.show();
		player_table.show();
	}
	
	void UILobby::unload_ui()
	{
		menu.show(false);
		player_table.show(false);
	}
	
	void UILobby::update()
	{
		using namespace tab;
		
		const Lobby& l = app->lobby;
		
		player_table.clear_tab(0);
		
		for(NetPlayer p : l.player)
		{
			Row r = player_table.add_row();
			r[0] << p.name;
			r[1] << (p.ready ? "(R)" : "( )");
		}
	}
	
	void UILobby::cb_ready()
	{
		Lobby& l = app->lobby;
		
		if(net::tcp::getstate() == net::SERVER)
		{
			NetPlayer* p = l.get_player(l.my_id);
			p->ready = true;
			if(l.all_ready())
			{
				app->host_start_game();
			}
			else
			{
				app->host.send_lobby_data();
				update();
			}
		}
		else if(net::tcp::getstate() == net::CLIENT)
		{
			app->client.send_ready();
		}
	}
			
		
		
