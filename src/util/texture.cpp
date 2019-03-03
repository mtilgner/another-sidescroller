	#include "texture.h"
	
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_image.h>
	#include <iostream>
	#include <vector>
	
	
	GLuint TextureManager::load(const char* png_file)
	{
		auto it = tex_.find(png_file);
		if(it != tex_.end())
		{
			std::cout << "image already loaded: " << png_file << "\n";
			return it->second;
		}
		std::string full_name = png_file;
		full_name = full_name + ".png";
		SDL_Surface* s = IMG_Load(full_name.c_str());
		
		if(!s)
		{
			std::cout << "couldn't load image: " << png_file << "\n";
			return 0;
		}
		
		
		GLuint t = texture_from_surface(s);
		SDL_FreeSurface(s);
		tex_[png_file] = t;
		
		return t;
	}
	
	std::list<std::string> TextureManager::loaded_texture_names() const
	{
		std::list<std::string> L;
		
		for(const auto& p : tex_)
		{
			L.push_back(p.first);
		}
		return L;
	}
			
	
	void TextureManager::unload()
	{
		for(auto& p : tex_)
		{
			glDeleteTextures(1, &p.second);
		}
		tex_.clear();
	}
	
	GLuint TextureManager::texture(const char* name)
	{
		auto it = tex_.find(name);
		if(it != tex_.end())
		{
			return it->second;
		}
		return 0;
	}
	
	GLuint TextureManager::texture_load(const char* name)
	{
		GLuint t = texture(name);
		if(!t)
		{
			t = load(name);
		}
		return t;
	}
	
	TextureManager& TextureManager::get()
	{
		static TextureManager tm;
		return tm;
	}
	
	Uint32 get_pixel(SDL_Surface* surface, int x, int y)
	{
		Uint8* p = (Uint8*) surface->pixels;
		Uint8* hit_p = p + y * surface->pitch + x * 4;
		Uint32 pixel = *((Uint32*) hit_p);
		return pixel;
	}
	
	GLuint texture_from_surface(SDL_Surface* src)
	{
		GLuint texture = 0;
		GLenum tex_fmt;
		
		std::vector<Uint32> v(src->w * src->h);
		for(int i = 0; i < v.size(); ++i)
		{
			v[i] = get_pixel(src, i % src->w, i / src->w);
		}

		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D,texture);

		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		
		tex_fmt = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,src->w,src->h,0,tex_fmt,GL_UNSIGNED_BYTE,&v.front());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,0);
		
		return texture;
	}
