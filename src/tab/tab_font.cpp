	

	#ifndef MATE_COMPILE
	#include "tab_font.h"
	#endif

	#include <iostream>

	namespace tab
	{

	//		Font

	

	void Font::color(unsigned char r, unsigned char g, unsigned char b)
	{
		r_ = r;
		g_ = g;
		b_ = b;
	}

	void Font::alpha(unsigned char a)
	{
		a_ = a;
	}

	
	
	void print(Font* font, const SDL_Rect& dest, TextAlign align, std::string text)
	{
		int x = 0, y = 0;
		SDL_Rect ts = font->text_size(text);
		
		switch(align)
		{
			case ALIGN_LEFT:
				x = dest.x;
				break;
			
			case ALIGN_RIGHT:
				x = dest.x + dest.w - ts.w;
				break;
			
			case ALIGN_CENTER:
				x = dest.x + (dest.w - ts.w) / 2;
				break;
		}
		
		y = dest.y + (dest.h - ts.h) / 2;
		
		font->print(x, y, text);
	}
				



	// SDLFont
	
	SDL_Renderer* SDLFont::renderer_ = 0;

	void SDLFont::init(SDL_Renderer* renderer)
	{
		renderer_ = renderer;
	}
	
	SDLFont::~SDLFont()
	{
		clear();
	}
		
	void SDLFont::load(TTF_Font* ttf_font)
	{
		
		clear();
		
		static SDL_Color white = {255, 255, 255, 255};
		
		h_ = TTF_FontHeight(ttf_font);
		int adv = 0;
		
		for(int i = 0; i < 256; ++i)
		{
			char ch = TTF_GlyphIsProvided(ttf_font, i) ? i : ' ';
			TTF_GlyphMetrics(ttf_font, ch, 0, 0, 0, 0, &adv);
			SDL_Surface* glyph = TTF_RenderGlyph_Blended(ttf_font, ch, white);
			
			SDL_Texture* t = SDL_CreateTextureFromSurface(renderer_, glyph);
			tex_.push_back(t);
			adv_.push_back(adv);
			
			SDL_FreeSurface(glyph);
		}
	}

	void SDLFont::clear()
	{
		r_ = g_ = b_ = a_ = 255;
		for(SDL_Texture* t : tex_)
		{
			SDL_DestroyTexture(t);
		}
		tex_.clear();
		adv_.clear();
		h_ = 0;
		

	}
	
	void SDLFont::print(int x, int y, std::string text)
	{	
		SDL_Rect dest;
		dest.x = x;
		dest.y = y;
		dest.h = h_;
		
		for(char t : text)
		{
			SDL_SetTextureColorMod(tex_[t], r_, g_, b_);
			SDL_SetTextureAlphaMod(tex_[t], a_);
			dest.w = adv_[t];
			SDL_RenderCopy(renderer_, tex_[t], 0, &dest);
			dest.x += dest.w;
		}
	}
	

	void SDLFont::draw_rect(const SDL_Rect& r)
	{
		SDL_SetRenderDrawColor(renderer_, r_, g_, b_, a_);
		SDL_RenderFillRect(renderer_, &r);
		// SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
	}
	
	SDL_Rect SDLFont::text_size(std::string text)
	{
		SDL_Rect ts;
		ts.x = 0;
		ts.y = 0;
		ts.w = 0;
		ts.h = h_;
		
		for(char t : text)
		{
			ts.w += adv_[t];
		}
		
		return ts;
	}
	
	#ifdef TAB_COMPILE_OPENGL_FONT
	
	OpenGLFont::~OpenGLFont()
	{
		clear();
	}
	
	GLuint TextureFromSurface(SDL_Surface* src)
	{
		GLuint texture;
		GLenum tex_fmt;

		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D,texture);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
		tex_fmt = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,src->w,src->h,0,tex_fmt,GL_UNSIGNED_BYTE,src->pixels);
		glBindTexture(GL_TEXTURE_2D,0);

		return texture;
	}

	void OpenGLFont::load(TTF_Font* ttf_font)
	{
		clear();
		
		static SDL_Color white = {255, 255, 255, 255};
		
		h_ = TTF_FontHeight(ttf_font);
		int adv = 0;
		
		for(int i = 0; i < 256; ++i)
		{
			char ch = TTF_GlyphIsProvided(ttf_font, i) ? i : ' ';
			TTF_GlyphMetrics(ttf_font, ch, 0, 0, 0, 0, &adv);
			SDL_Surface* glyph = TTF_RenderGlyph_Solid(ttf_font, ch, white);
			SDL_Surface* srf = SDL_CreateRGBSurface(0, adv, h_, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
			SDL_FillRect(srf, 0, 0);
			SDL_BlitSurface(glyph, 0, srf, 0);
			GLuint t = TextureFromSurface(srf);
//			GLuint t = 0;
			tex_.push_back(t);
			adv_.push_back(adv);
			
			SDL_FreeSurface(glyph);
			SDL_FreeSurface(srf);
		}
	}
	
	void OpenGLFont::clear()
	{
		for(GLuint t : tex_)
		{
			glDeleteTextures(1, &t);
		}
		tex_.clear();
		adv_.clear();
		h_ = 0;
	}
	
	void OpenGLFont::print(int x, int y, std::string text)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslatef((GLfloat) x, (GLfloat) y, 0.0f);
		glColor4ub(r_, g_, b_, a_);

		for(char c : text)
		{
			float width = (float) adv_[c];
			float height = (float) h_;
			
			glBindTexture(GL_TEXTURE_2D, tex_[(unsigned) c]);
			
			glBegin(GL_QUADS);
			
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(width, 0.0f);
			
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(width, height);
			
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, height);
			
			glEnd();
			
			glTranslatef((GLfloat) adv_[c], 0.0f, 0.0f);
		}
		
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}
	
	void OpenGLFont::draw_rect(const SDL_Rect& r)
	{
		glPushMatrix();
		glLoadIdentity();
		glColor4ub(r_, g_, b_, a_);
		
		GLfloat
		x = (GLfloat) r.x,
		y = (GLfloat) r.y,
		w = (GLfloat) r.w,
		h = (GLfloat) r.h;
		
		glBegin(GL_QUADS);
		
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		
		glEnd();
		
		glPopMatrix();
	}
	
	SDL_Rect OpenGLFont::text_size(std::string text)
	{
		SDL_Rect ts;
		ts.x = 0;
		ts.y = 0;
		ts.w = 0;
		ts.h = h_;
		
		for(char t : text)
		{
			ts.w += adv_[t];
		}
		
		return ts;
	}	
	
	#endif
	
	} // namespace tab

