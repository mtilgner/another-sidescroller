	#include "settings.h"
	#include <fstream>
	
	void Settings::load()
	{
		std::ifstream is("settings.ini");
		
		std::string name;
		while(is)
		{
			is >> name;
			if(name == "IP")
			{
				is >> ip;
			}
			else if(name == "Port")
			{
				is >> port;
			}
			else if(name == "Name")
			{
				is >> player_name;
			}
			else if(name == "Resolution")
			{
				is >> screen_w >> screen_h;
			}
			else if(name == "Fullscreen")
			{
				is >> fullscr;
			}
		}
	}
