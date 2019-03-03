	#include "application.h"
	#include "shader.h"
	#include "texture.h"
	#include "animation.h"
	
	#include <iostream>
	#include <sstream>
	
	
	void Application::init()
	{
		settings.load();
		unsigned window_flags = settings.fullscr ? SDL_WINDOW_FULLSCREEN : 0;
		
		SDL_Init(SDL_INIT_VIDEO);
		
		window = SDL_CreateWindow(
		"AnotherFuture",
		 SDL_WINDOWPOS_UNDEFINED,
		 SDL_WINDOWPOS_UNDEFINED,
		 settings.screen_w,
		 settings.screen_h, 
		 window_flags);
		
		renderer = SDL_CreateRenderer(window, -1, 0);
		glcontext = SDL_GL_CreateContext(window);
		
		SDL_GL_SetSwapInterval(0);
		
		glewInit();
		
		glViewport(0, 0, (GLsizei) settings.screen_w, (GLsizei) settings.screen_h);
		
		glEnable(GL_TEXTURE_2D);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        
        if(!init_shader(settings.screen_w, settings.screen_h, game.map2d.tiledim))
        {
			ShaderError error = get_shader_error();
			std::cout << error.error << "\n" << error.info_log << "\n";
		}
        use_projection_ui();
		
		host.init();
		client.init();
		
		TTF_Init();
		TTF_Font* ttf_font = TTF_OpenFont("LucidaTypewriterRegular.ttf", 14);
		
		ui.init(ttf_font, renderer);

		TTF_CloseFont(ttf_font);
		TTF_Quit();
		
		ui_main.init();
		ui_lobby.init();
		ui_debug_log.init();
		
		ui_main.load_ui();
		//ui_debug_log.load_ui();
	}
	
	void Application::host_start_lobby()
	{
		net::tcp::start_server(settings.port);
		NetPlayer p;
		p.id = 1;
		p.name = settings.player_name;
		lobby.player.push_back(p);
		lobby.my_id = p.id;
		
		ui_main.unload_ui();
		ui_lobby.load_ui();
		ui_lobby.update();
		
		state = LOBBY;
	}
	
	void Application::client_start_lobby()
	{
		net::tcp::start_client(settings.ip.c_str(), settings.port);
		client.send_hello();
		
		ui_main.unload_ui();
		ui_lobby.load_ui();
		ui_lobby.update();
		
		state = LOBBY;
	}
	
	void Application::host_start_game()
	{
		//dummy
		//load game
		//make clients unready
		ui_lobby.unload_ui();
		ui_debug_log.print("game started");
		
		
		state = GAME;
		
		game.init_server();
		game.start(SDL_GetTicks());
		
		host.send_start_game();
		
	}
	
	void Application::client_start_game()
	{
		//dummy
		//load game
		ui_lobby.unload_ui();
		ui_debug_log.print("game started");
		
		state = GAME;
		
		game.init_client();
		game.start(SDL_GetTicks());
	}
	
	void Application::quit()
	{
		if(net::tcp::getstate() == net::SERVER)
		{
			host.send_bye();
		}
		else if(net::tcp::getstate() == net::CLIENT)
		{
			client.send_bye();
		}
		net::tcp::disconnect();
		quit_flag = true;
	}
	
	void Application::return_to_main()
	{
		using namespace net;
		
		if(net::tcp::getstate() == SERVER)
		{
			host.send_bye();
		}
		else if(net::tcp::getstate() == CLIENT)
		{
			client.send_bye();
		}
		net::tcp::disconnect();
		lobby.clear();
		
		if(state == LOBBY)
		{
			ui_lobby.unload_ui();
		}
		else if(state == GAME)
		{
			// unload game
		}
		
		ui_main.load_ui();
		state = MAIN;
	}
	
	void Application::handle_network()
	{
		net::Message msg;
		
		if(net::tcp::receive())
		{
			while(net::poll_message(msg))
			{
				if(net::tcp::getstate() == net::SERVER)
				{
					host.handle_msg(msg);
				}
				else if(net::tcp::getstate() == net::CLIENT)
				{
					client.handle_msg(msg);
				}
			}
		}
	}
	
	void Application::loop()
	{
		using namespace tab;
		
		SDL_Event e;
		while(!quit_flag)
		{
			if(net::tcp::getstate() != net::DISCONNECTED)
			{
				handle_network();
			}
			
			while(SDL_PollEvent(&e))
			{
				
				if(e.type == SDL_QUIT)
				{
					quit();
				}
				else
				{
					if(state == GAME)
					{
						game.handle(&e);
					}
					Table::handle(&e);
					Table::logic();
				}
			}
			if(state == GAME)
			{
				game.handle(0);
				int time = SDL_GetTicks();
				game.update(time);
			}
			
			if(state == GAME)
			{
				use_projection_game();
				use_color(1.0f, 1.0f, 1.0f, 1.0f);
				game.draw();
			}
			
			use_projection_ui();
			use_color(1.0f, 1.0f, 1.0f, 1.0f);
			Table::draw();
			
		
			glFinish();
			SDL_GL_SwapWindow(window);
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}
	
	void Application::exit()
	{
		game.clear();
		ui.quit();
		PropType::clear();
		clear_shader();
		TextureManager::get().unload();
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	
	Application& Application::get()
	{
		static Application app;
		return app;
	}
		
