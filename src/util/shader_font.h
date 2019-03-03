	#ifndef SHADER_FONT_H
	#define SHADER_FONT_H
	
	#include "tab_font.h"
	
	
	class ShaderFont : public tab::Font
	{
	public:
		~ShaderFont();
		void load(TTF_Font* ttf_font);
		void clear();
		
		void print(int x, int y, std::string text);
		void draw_rect(const SDL_Rect& r);
		
		SDL_Rect text_size(std::string text);
	private:
		std::vector<GLuint> tex_, vbo_;
		std::vector<unsigned> adv_;
		GLuint rect_vbo_ = 0;
		unsigned h_ = 0;
	};
	
	
	
	
	
	
	
	
	
	#endif
	
	
