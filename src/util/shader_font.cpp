	#include "shader_font.h"
	#include "texture.h"
	#include "shader.h"
	
	ShaderFont::~ShaderFont()
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

	void ShaderFont::load(TTF_Font* ttf_font)
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
			
			GLuint t = texture_from_surface(srf);
			GLuint vbo = gen_vbo_ui(adv, h_);
			
			tex_.push_back(t);
			vbo_.push_back(vbo);
			adv_.push_back(adv);
			
			SDL_FreeSurface(glyph);
			SDL_FreeSurface(srf);
		}
		
		rect_vbo_ = gen_vbo_ui(1.0f, 1.0f, true);
	}
	
	void ShaderFont::clear()
	{
		for(GLuint t : tex_)
		{
			glDeleteTextures(1, &t);
		}
		tex_.clear();
		for(GLuint b : vbo_)
		{
			glDeleteBuffers(1, &b);
		}
		vbo_.clear();
		glDeleteBuffers(1, &rect_vbo_);
		adv_.clear();
		h_ = 0;
	}
	
	void ShaderFont::print(int x, int y, std::string text)
	{
		use_color((float) r_ / 255.0f, (float) g_ / 255.0f, (float) b_ / 255.0f, (float) a_ / 255.0f);
		for(char c : text)
		{
			float width = (float) adv_[c];
			float height = (float) h_;
			draw_vbo_texture(vbo_[c], tex_[c], x, y);	
			x += adv_[c];
		}
	}
	
	void ShaderFont::draw_rect(const SDL_Rect& r)
	{
		use_color((float) r_ / 255.0f, (float) g_ / 255.0f, (float) b_ / 255.0f, (float) a_ / 255.0f);
		
		GLfloat
		x = (GLfloat) r.x,
		y = (GLfloat) r.y,
		w = (GLfloat) r.w,
		h = (GLfloat) r.h;
		
		change_vertices_ui(rect_vbo_, w, h);
		draw_vbo_texture(rect_vbo_, 0, x, y);
	}
	
	SDL_Rect ShaderFont::text_size(std::string text)
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
