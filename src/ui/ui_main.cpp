	#include "ui_main.h"
	#include "ui.h"
	#include "application.h"
	
	void UIMain::init()
	{
		using namespace tab;
		
		app = &Application::get();
		
		TableAttributes ta;
		SDL_Rect rect = {50, 50, 300, 75};
		ta.rect = rect;
		ta.visible_rows = 3;
		ta.cell_border = 5;
		ta.table_border = 5;
		
		main_menu.set_attributes(ta);
		main_menu.show(false);
	}
	
	void UIMain::load_ui()
	{
		using namespace tab;
		
		main_menu.clear_tab(0);
		
		Row r = main_menu.add_row();
		make_button(r[0]);
		r[0] << "Host";
		r[0].handle = std::bind(&UIMain::cb_host, this);
		
		r = main_menu.add_row();
		make_button(r[0]);
		r[0] << "Join";
		r[0].handle = std::bind(&UIMain::cb_join, this);
		
		r = main_menu.add_row();
		make_button(r[0]);
		r[0] << "Quit";
		r[0].handle = std::bind(&cb_quit);
		
		main_menu.show();
	}
	
	void UIMain::unload_ui()
	{
		main_menu.show(false);
	}
	
	void UIMain::cb_host()
	{	
		app->host_start_lobby();
	}
	
	void UIMain::cb_join()
	{
		app->client_start_lobby();
	}
		
