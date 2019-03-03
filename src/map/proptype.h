	#ifndef PROPTYPE_H
	#define PROPTYPE_H
	
	#include <GL/glew.h>
	#include <vector>
	#include <map>
	#include <string>
	
	
	enum Facing
	{
		FACE_RIGHT,
		FACE_LEFT
	};
	
	struct PropLayer
	{
		float paralax = 1.0f;
	};
	
	struct PropType
	{
		int hit_w = 0, hit_h = 0;
		std::string name;
		float img_w = 0.0f, img_h = 0.0f, img_offs_x = 0.0f, img_offs_y = 0.0f;
		int layer = 0;
		GLuint vbo = 0, vbo_mirrored = 0, tex = 0;
		int id = 0;
		
		bool ladder_flag = false;
		
		void draw(float x, float y, int face = FACE_RIGHT);
		
		static void load(std::string file);
		static void clear();
		static int get_by_name(std::string name);
		
		static std::vector<PropType> all;
		static std::vector<PropLayer> layers;
		static int wall_layer_index;
		static std::map<std::string, int> index_table;
	};
		
	
	
	
	
	#endif
