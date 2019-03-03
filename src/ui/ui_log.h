	#ifndef UI_LOG_H
	#define UI_LOG_H
	
	#include "tab_table.h"
	
	struct Application;
	
	struct UILog
	{
		Application* app = 0;
		tab::Table log_table;
		
		void init();
		void load_ui();
		void unload_ui();
		void print(std::string str);
	};
		
	
	
	
	
	#endif
