	#ifndef APPLICATION_H
	#define APPLICATION_H
	
	#include "settings.h"
	#include "host.h"
	#include "client.h"
	#include "lobby.h"
	#include "ui.h"
	#include "ui_main.h"
	#include "ui_lobby.h"
	#include "ui_log.h"
	#include "Net.h"
	#include "game.h"
	
	enum AppState
	{
		MAIN,
		LOBBY,
		GAME
	};
	
	struct Application
	{
		unsigned state = MAIN;
		bool quit_flag = false;
		SDL_Renderer* renderer = 0;
		SDL_Window* window = 0;
		SDL_GLContext glcontext;
		Settings settings;
		Host host;
		Client client;
		Lobby lobby;
		UI ui;
		UIMain ui_main;
		UILobby ui_lobby;
		UILog ui_debug_log;
		
		Game game;
		
		void init();
		
		void host_start_lobby();
		void client_start_lobby();
		
		void host_start_game();
		void client_start_game();
		
		void quit();
		void return_to_main();
		
		void handle_network();
		
		void loop();
		
		void exit();

		static Application& get();
	};
		
		
	
	
	
	
	#endif
