	#ifndef TAB_FONT_H
	#define TAB_FONT_H

	#define TAB_COMPILE_OPENGL_FONT
	
	#include <SDL2/SDL_ttf.h>
	
	#ifdef TAB_COMPILE_OPENGL_FONT
	#include <GL/glew.h>
	#endif
	
	#include <SDL2/SDL.h>
	#include <vector>
	#include <string>

	namespace tab
	{
	
	enum TextAlign
	{
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};
		
	class Font
	{
	public:
		virtual ~Font(){}

		virtual void print(int x, int y, std::string text) = 0;
		virtual void draw_rect(const SDL_Rect& r) = 0;

		virtual SDL_Rect text_size(std::string text) = 0;


		void color(unsigned char r, unsigned char g, unsigned char b);
		void alpha(unsigned char a);
	protected:
		unsigned char r_ = 255, g_ = 255, b_= 255, a_ = 255;
	};
	

//	void load_ttf(Font* font, TTF_Font* ttf_font);
	
	void print(Font* font, const SDL_Rect& dest, TextAlign align, std::string text);

		
		
	class SDLFont : public Font
	{
	public:
		static void init(SDL_Renderer* renderer);
		
		~SDLFont();
		
		void load(TTF_Font* ttf_font);
		void clear();

		void print(int x, int y, std::string text);
		void draw_rect(const SDL_Rect& r);

		SDL_Rect text_size(std::string text);
	private:
		std::vector<SDL_Texture*> tex_;
		std::vector<unsigned> adv_;
		unsigned h_ = 0;
		static SDL_Renderer* renderer_;
	};
	
	#ifdef TAB_COMPILE_OPENGL_FONT
	
	class OpenGLFont : public Font
	{
	public:
		~OpenGLFont();
		
		void load(TTF_Font* ttf_font);
		void clear();
		
		void print(int x, int y, std::string text);
		void draw_rect(const SDL_Rect& r);
		
		SDL_Rect text_size(std::string text);
	private:
		std::vector<GLuint> tex_;
		std::vector<unsigned> adv_;
		unsigned h_ = 0;
	};
		
	#endif


	} // namespace tab



	#endif
