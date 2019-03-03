	#ifndef UI_MAIN_H
	#define UI_MAIN_H
	
	#include "tab_table.h"
	
	struct Application;
	
	struct UIMain
	{
		Application* app = 0;
		tab::Table main_menu;
		
		void init();
		void load_ui();
		void unload_ui();
		
		void cb_host();
		void cb_join();
	};
	
	
	#endif
