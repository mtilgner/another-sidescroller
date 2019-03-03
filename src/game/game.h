	#ifndef GAME_H
	#define GAME_H
	
	#include "map2d.h"
	#include "character.h"
	
	#include <map>
	#include <functional>
	
	union SDL_Event;
	struct Application;
	
	struct Game
	{
		int current_tick = 0;
		std::map<int, CharacterInput> current_inputs, new_inputs;
		std::map<int, Character> players;
		CharacterInput next_input;
		int last_tick_ms = 0, delay_ms = 50;
		Map2D map2d;
		
		std::function<void (Game*)> on_tick = 0;
		
		Application* app = 0;
		
		void init_server();
		void init_client();
		
		void start(int ms);
		
		void handle(SDL_Event* e);
		void update(int ms); // update non-network data, trigger tick
		void do_tick(int ms); // update network data
		
		void draw();
		
		bool tick_ready(int ms);
		bool players_ready();
		
		void on_tick_server();
		void on_tick_client();
		
		void clear();
	};
		
	
	
	
	
	
	
	
	
	#endif
