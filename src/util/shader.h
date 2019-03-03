	#ifndef SHADER_H
	#define SHADER_H
	
	#include <string>
	#include <GL/glew.h>
	
	struct ShaderError
	{
		std::string error, info_log;
	};
	
	enum ShaderProgram
	{
		PROG_TEXTURE,
		PROG_COLOR,
		PROG_TEXTURE_COLOR,
		
		PROG_SIZE
	};
	
	bool init_shader(float screen_w, float screen_h, float tiledim);
	void clear_shader();
	
	void prepare_buffer();
	void select_texture(GLuint texture);
	void draw_vbo(GLuint vbo, float x, float y);
	void draw_vbo_texture(GLuint vbo, GLuint texture, float x, float y);
	
	GLuint gen_vbo_simple(float w, float h, float x = 0.0f, float y = 0.0f); // from (x, y) to (x + w, y + h), draws entire texture
	GLuint gen_vbo_simple_mirrored(float w, float h, float x = 0.0f, float y = 0.0f);
	GLuint gen_vbo_ui(float w, float h, bool dynamic = true);
	
	void change_vertices_ui(GLuint dynamic_vbo, float w, float h, float x = 0.0f, float y = 0.0f);
	
	void use_projection_ui();
	void use_projection_game();
	void use_color(float r, float g, float b, float a);
	void use_program(int shader_program);
	
	ShaderError get_shader_error();
	
	
	
	
	
	
	
	#endif
	
