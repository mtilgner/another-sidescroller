	#ifndef TEXTURE_H
	#define TEXTURE_H
	
	#include <GL/glew.h>
	#include <string>
	#include <map>
	#include <list>
	
	struct SDL_Surface;
	
	class TextureManager
	{
	public:
		
		GLuint load(const char* png_file);
		void unload();
		GLuint texture(const char* name);
		GLuint texture_load(const char* name);
		
		std::list<std::string> loaded_texture_names() const;
		
		static TextureManager& get();
	private:
		std::map<std::string, GLuint> tex_;
	};
	
	GLuint texture_from_surface(SDL_Surface* src);
	
		
	
	
	
	
	#endif
