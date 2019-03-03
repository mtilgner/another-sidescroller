	#include "game.h"
	#include "application.h"
	#include "animation.h"
	
	#include <SDL2/SDL.h>
	#include <algorithm>
	#include <iostream>
	

	void Game::init_server()
	{
		on_tick = &Game::on_tick_server;
		app = &Application::get();
	}
	
	void Game::init_client()
	{
		on_tick = &Game::on_tick_client;
		app = &Application::get();
	}
	
	void Game::start(int ms)
	{
		clear();
		
		for(const auto& np : app->lobby.player)
		{
			Character& c = players[np.id];
			c.init_character("pixelman/p");
			
			c.pos = GEOM::Vec2::null;
			//c.pos.x = -20.0f;
			
			current_inputs[np.id] = CharacterInput();
			new_inputs[np.id] = CharacterInput();
		}
		
		PropType::load("props.txt");
		map2d.load("map.txt");
		
		last_tick_ms = ms;
	}
	
	void Game::handle(SDL_Event* e)
	{
		const Uint8* keys = SDL_GetKeyboardState(0);
		CharacterInput& in = next_input;
		
		int mouse_x, mouse_y;
		Uint32 button = SDL_GetMouseState(&mouse_x, &mouse_y);
		map2d.project_coords(mouse_x, mouse_y, in.point.x, in.point.y);
		if(button & SDL_BUTTON(SDL_BUTTON_RIGHT))
		{
			in.action = 1;
		}
		else
		{
			in.action = 0;
		}
		
		if(e && e->type == SDL_KEYDOWN)
		{
			if(e->key.keysym.sym == SDLK_ESCAPE)
			{
				clear();
				app->return_to_main();
			}
		}
	}
	
	void Game::update(int ms)
	{
		for(auto& p : players)
		{
			p.second.anim_current->update(ms);
		}
		
		if(tick_ready(ms))
		{
			do_tick(ms);
		}
	}
	
	void Game::do_tick(int ms)
	{
		for(auto& p : players)
		{
			CharacterInput in = current_inputs[p.first];
			p.second.enter_input(map2d, in, ms);
			p.second.update_position(map2d, ms);
		}
		last_tick_ms = ms;
		
		// server: enter new_cstate into new_cstates
		// client: send new_cstate to server
		
		++current_tick;
		next_input.tick = current_tick;
		std::swap(new_inputs, current_inputs);
		
		on_tick(this);
		
		
	}
	
	
	void Game::draw()
	{
		Character& local = players[app->lobby.my_id];
		map2d.draw(-local.pos.x, -local.pos.y);
		
		for(auto& p : players)
		{
			if(p.first != app->lobby.my_id)
			{
				p.second.anim_current->draw(p.second.pos.x - local.pos.x, p.second.pos.y - local.pos.y);
			}
			else
			{
				p.second.anim_current->draw(0.0f, 0.0f);
			}
		}
	}
	
	bool Game::tick_ready(int ms)
	{
		if(ms - last_tick_ms >= delay_ms)
		{
			return players_ready();
		}
		return false;
	}
	
	bool Game::players_ready()
	{
		for(auto& p : current_inputs)
		{
			if(p.second.tick != current_tick)
			{
				//std::cout << "player " << p.first << " not ready \n";
				return false;
			}
		}
		return true;
	}
	
	void Game::clear()
	{
		for(auto& p : players)
		{
			p.second.clear_character();
		}
		players.clear();
		current_inputs.clear();
		new_inputs.clear();
		current_tick = 0;
	}
	
	void Game::on_tick_server()
	{
		//std::cout << "tick: " << current_tick << " entering state\n";
		current_inputs[app->lobby.my_id] = next_input;
		if(players_ready())
		{
			app->host.send_input_all();
		}
	}
	
	void Game::on_tick_client()
	{
		//std::cout << "tick: " << current_tick << " sending state\n";
		app->client.send_input_single();
	}
	
