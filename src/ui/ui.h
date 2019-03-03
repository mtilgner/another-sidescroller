	#ifndef UI_H
	#define UI_H
	
	#include "tab_table.h"
	#include "shader_font.h"
	
	struct UI
	{
		ShaderFont font;
		void init(TTF_Font* ttf_font, SDL_Renderer* renderer);
		void quit();
	};
	
	void cb_quit();
	void make_button(tab::Cell& c);
	
	
	
	#endif
