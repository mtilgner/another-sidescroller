	#ifndef UI_LOBBY_H
	#define UI_LOBBY_H
	
	#include "tab_table.h"
	
	struct Application;
	
	struct UILobby
	{
		Application* app = 0;
		tab::Table player_table, menu;
		
		void init();
		
		void load_ui();
		void unload_ui();
		void update();
		
		void cb_ready();
	};
		
	
	
	#endif
