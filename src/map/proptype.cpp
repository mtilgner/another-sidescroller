	#include "proptype.h"
	#include "shader.h"
	#include "texture.h"
	
	#include <string>
	#include <fstream>
	#include <sstream>
	#include <iostream>
	
	#define PROPTYPE_MARKER '@'
	
	std::vector<PropType> PropType::all;
	std::vector<PropLayer> PropType::layers;
	int PropType::wall_layer_index = 0;
	std::map<std::string, int> PropType::index_table;
	
	void PropType::clear()
	{
		for(PropType& pt : all)
		{
			glDeleteBuffers(1, &pt.vbo);
			glDeleteBuffers(1, &pt.vbo_mirrored);
		}
		all.clear();
	}
	
	int PropType::get_by_name(std::string name)
	{
		auto it = index_table.find(name);
		if(it != index_table.end())
		{
			return it->second;
		}
		std::cout << "PropType not found: " << name << "\n";
		return -1;
	}
		
	
	void load_prop_type(std::vector<std::istringstream>& lines, PropType& pt)
	{
		std::string keyword;
		TextureManager& texman = TextureManager::get();
		bool isize_set = false;
		for(std::istringstream& line : lines)
		{
			
			line >> keyword;
			
			if(keyword == "texture")
			{
				std::string tname;
				line >> tname;
				pt.tex = texman.texture_load(tname.c_str());
			}
			else if(keyword == "size")
			{
				line >> pt.hit_w >> pt.hit_h;
			}
			else if(keyword == "isize")
			{
				line >> pt.img_w >> pt.img_h >> pt.img_offs_x >> pt.img_offs_y;
				isize_set = true;
			}
			else if(keyword == "ladder")
			{
				pt.ladder_flag = true;
			}
		}
		if(!isize_set)
		{
			pt.img_w = pt.hit_w;
			pt.img_h = pt.hit_h;
		}
		
		pt.vbo = gen_vbo_simple(pt.img_w, pt.img_h, pt.img_offs_x, pt.img_offs_y);
		pt.vbo_mirrored = gen_vbo_simple_mirrored(pt.img_w, pt.img_h, pt.img_offs_x, pt.img_offs_y);
		
		
	}
	
	void PropType::load(std::string file)
	{	
		std::ifstream in(file.c_str());
		
		std::string item;
		std::vector<std::string> items;
		
		std::getline(in, item, PROPTYPE_MARKER);
		while(std::getline(in, item, PROPTYPE_MARKER))
		{
//			std::cout << "item " << items.size() << "\n";
			items.push_back(item);
		}
		int layer = 0;
		
		for(std::string str : items)
		{
			std::istringstream iss(str);
			std::string keyword;
			iss >> keyword;
			if(keyword == "LAYER" || keyword == "LAYER*")
			{
				PropLayer l;
				iss >> l.paralax;
				layer = layers.size();
				if(keyword == "LAYER*")
				{
					wall_layer_index = layer; 
				}
				layers.push_back(l);
			}
			else if(keyword == "PROP")
			{
				PropType pt;
				iss >> pt.name;
				pt.layer = layer;
				std::string line;
				std::getline(iss, line); // extract the enter
				std::vector<std::istringstream> lines;
				while(std::getline(iss, line))
				{
					lines.push_back(std::istringstream(line));
				}
				load_prop_type(lines, pt);
				pt.id = all.size();
				all.push_back(pt);
				index_table[pt.name] = pt.id;
			}
		}
	}
				
				
			
