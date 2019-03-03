
	#include "shader.h"
	#include <string.h>
	
	static const char *vs_source =
	"#version 120\n" // OpenGL 2.1
	"attribute vec2 a_coord2d;"
	"attribute vec2 a_tex;"
	"uniform vec2 u_pos;"
	"uniform mat4 u_proj;"
	"varying vec2 v_tex_coord;"
	"void main(void) { "
	"gl_Position = u_proj * (vec4(u_pos + a_coord2d, 0.0, 1.0));"
	"v_tex_coord = a_tex;"
	"}";
	
	static const char *fs_source =
	"#version 120\n" // OpenGL 2.1
	"varying vec2 v_tex_coord;"
	"uniform vec4 u_color;"
	"uniform sampler2D u_tex;"
	"void main(void) { "
	"vec4 color = texture2D(u_tex, v_tex_coord);"
	"gl_FragColor = vec4(color.r * u_color.r, color.g * u_color.g, color.b * u_color.b, color.a * u_color.a);"
	"}";
	
	static ShaderError last_error;
	static GLuint vertex_shader = 0, fragment_shader = 0, program = 0;
	static float color[4] = {0};
	
	static GLint attribute_coord2d = 0;
	static GLint attribute_tex = 0;

	static GLint uniform_proj = 0;
	static GLint uniform_pos = 0;
	static GLint uniform_tex = 0;
	static GLint uniform_color = 0;
	
	struct Mat4
	{
		float data[16];
		static Mat4 null()
		{
			Mat4 m;
			memset(m.data, 0, sizeof(float) * 16);
			return m;
		}
		float& entry(unsigned row, unsigned col)
		{
			return data[4 * col + row];
		}
		
		const float& entry(unsigned row, unsigned col) const
		{
			return data[4 * col + row];
		}
	};
	
	static Mat4 projection_matrix_ui = Mat4::null(), projection_matrix_game = Mat4::null();

	Mat4 operator*(const Mat4& m1, const Mat4& m2)
	{
		Mat4 r;
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				float s = 0.0f;
				for(int k = 0; k < 4; ++k)
				{
					s += m1.entry(i, k) * m2.entry(k, j);
				}
				r.entry(i, j) = s;
			}
		}
		return r;
	}
	
	void init_projection_matrix_ui(int w, int h)
	{
		projection_matrix_ui = Mat4::null();
		
		projection_matrix_ui.entry(0, 0) = 2.0f / w;
		projection_matrix_ui.entry(1, 1) = -2.0f / h;
		projection_matrix_ui.entry(2, 2) =  0.0f;//-2.0f / f;
		
		projection_matrix_ui.entry(0, 3) = -1.0f;
		projection_matrix_ui.entry(1, 3) = 1.0f;
		projection_matrix_ui.entry(2, 3) = 0.0f;//-1.0f;
		projection_matrix_ui.entry(3, 3) = 1.0f;		
	}
	
	void init_projection_matrix_game(float w, float h)
	{
		projection_matrix_game = Mat4::null();
		
		projection_matrix_game.entry(0, 0) = 2.0f / w;
		projection_matrix_game.entry(1, 1) = 2.0f / h;
		projection_matrix_game.entry(2, 2) =  0.0f;//-2.0f / f;
		
		projection_matrix_game.entry(0, 3) = 0.0f;
		projection_matrix_game.entry(1, 3) = 0.0f;
		projection_matrix_game.entry(2, 3) = 0.0f;//-1.0f;
		projection_matrix_game.entry(3, 3) = 1.0f;
	}
	
	
	
	bool init_shader(float screen_w, float screen_h, float tiledim)
	{
		static char info_log[10000];
		GLsizei len;
		
		GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
		
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		if(!vertex_shader)
		{
			last_error.error = "failed to create vertex shader";
			return false;
		}
		
		
		glShaderSource(vertex_shader, 1, &vs_source, NULL);
		glCompileShader(vertex_shader);
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_ok);
		if (compile_ok == GL_FALSE)
		{
			last_error.error = "vertex_shader compile failed";
			glGetShaderInfoLog(vertex_shader, 10000, &len, info_log);
			last_error.info_log = info_log;
			return false;
		}

		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		if(!fragment_shader)
		{
			last_error.error = "failed to create fragment shader";
			return false;
		}
		
		glShaderSource(fragment_shader, 1, &fs_source, NULL);
		glCompileShader(fragment_shader);
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_ok);
		if (!compile_ok)
		{
			last_error.error = "fragment shader compile failed";
			glGetShaderInfoLog(fragment_shader, 10000, &len, info_log);
			last_error.info_log = info_log;
			return false;
		}

		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
		if (!link_ok)
		{
			last_error.error = "link failed";
			return false;
		}
		
		glUseProgram(program);
		
		attribute_coord2d = glGetAttribLocation(program, "a_coord2d");
		attribute_tex = glGetAttribLocation(program, "a_tex");

		uniform_color = glGetUniformLocation(program, "u_color");	
		uniform_proj = glGetUniformLocation(program, "u_proj");	
		uniform_pos = glGetUniformLocation(program, "u_pos");
		uniform_tex = glGetUniformLocation(program, "u_tex");
		
		glEnableVertexAttribArray(attribute_coord2d);
		glEnableVertexAttribArray(attribute_tex);
		
		init_projection_matrix_ui(screen_w, screen_h);
		init_projection_matrix_game(screen_w / tiledim, screen_h / tiledim);
		
		for(float& c : color)
		{
			c = 1.0f;
		}
		
		return true;  
	}
	
	void prepare_buffer()
	{
		glVertexAttribPointer(
		attribute_coord2d,   // attribute
		2,                   // number of elements per vertex, here (x,y)
		GL_FLOAT,            // the type of each element
		GL_FALSE,            // take our values as-is
		4 * sizeof(GLfloat), // next coord2d appears every 6 floats
		0                    // offset of the first element
		);
		
		glVertexAttribPointer(
		attribute_tex,      	// attribute
		2,                      // number of elements per vertex, here (r,g,b)
		GL_FLOAT,               // the type of each element
		GL_FALSE,               // take our values as-is
		4 * sizeof(GLfloat),    // next color appears every 6 floats
		(GLvoid*) (2 * sizeof(GLfloat))  // offset of first element
		);
	}
	
	
	void draw_vbo_texture(GLuint vbo, GLuint texture, float x, float y)
	{	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glUniform1i(uniform_tex, 0);
		glUniform2f(uniform_pos, x, y);
		glUniform4fv(uniform_color, 1, color);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		prepare_buffer();
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
	}
	
	GLuint gen_vbo_simple(float w, float h, float x, float y)
	{
		GLuint buffer = 0;
		
		GLfloat quad_attributes[] = 
		{
			x, y,			0.0f, 1.0f,
			x + w, y, 		1.0f, 1.0f,
			x, y + h,		0.0f, 0.0f,
			x + w, y + h,	1.0f, 0.0f
		};
		
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_attributes), quad_attributes, GL_STATIC_DRAW);
		
		return buffer;
	}
	
	GLuint gen_vbo_simple_mirrored(float w, float h, float x, float y)
	{
		GLuint buffer = 0;
		
		GLfloat quad_attributes[] = 
		{
			x, y,			1.0f, 1.0f,
			x + w, y, 		0.0f, 1.0f,
			x, y + h,		1.0f, 0.0f,
			x + w, y + h,	0.0f, 0.0f
		};
		
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_attributes), quad_attributes, GL_STATIC_DRAW);
		
		return buffer;
	}
	
	GLuint gen_vbo_ui(float w, float h, bool dynamic)
	{
		GLuint buffer = 0;
		
		GLfloat quad_attributes[] = 
		{
			0.0f, 0.0f,			0.0f, 0.0f,
			w, 0.0f, 			1.0f, 0.0f,
			0.0f, h,			0.0f, 1.0f,
			w, h,				1.0f, 1.0f
		};
		
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_attributes), quad_attributes, dynamic? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		
		return buffer;
	}
	
	void change_vertices_ui(GLuint dynamic_vbo, float w, float h, float x, float y)
	{
		GLfloat quad_attributes[] = 
		{
			x, y,			0.0f, 0.0f,
			x + w, y, 		1.0f, 0.0f,
			x, y + h,		0.0f, 1.0f,
			x + w, y + h,	1.0f, 1.0f
		};
		
		glBindBuffer(GL_ARRAY_BUFFER, dynamic_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad_attributes), quad_attributes);
	}
	
	void clear_shader()
	{
		glDeleteProgram(program);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
	}
	
	void select_texture(GLuint texture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(uniform_tex, 0);
	}
	
	void draw_vbo(GLuint vbo, float x, float y)
	{
		glUniform2f(uniform_pos, x, y);
		glUniform4fv(uniform_color, 1, color);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		prepare_buffer();
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	void use_projection_ui()
	{
		glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, projection_matrix_ui.data);
	}
	
	void use_projection_game()
	{
		glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, projection_matrix_game.data);
	}
	
	void use_color(float r, float g, float b, float a)
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;
	}
	
	ShaderError get_shader_error()
	{
		return last_error;
	}
	
