	#ifndef SETTINGS_H
	#define SETTINGS_H
	
	#include <string>
	
	struct Settings
	{
		std::string ip;
		unsigned short port = 0;
		std::string player_name;
		
		unsigned screen_w = 800, screen_h = 800;
		int fullscr = 0;
		
		void load();
	};
		
	
	#endif
