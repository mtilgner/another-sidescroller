	#include "map2d.h"
	#include "animation.h"
	#include "application.h"
	
	#include <fstream>
	#include <sstream>
	#include <iostream>
	
	
	void Map2D::init(int xrad, int yrad)
	{
		x_radius = xrad;
		y_radius = yrad;
		wall_layer_tiles.resize(2 * xrad, 2 * yrad);
	}
		
	void Map2D::load(std::string map_file)
	{
		std::ifstream in(map_file.c_str());
		std::string line;
		int xrad = 0, yrad = 0;
		while(std::getline(in, line))
		{
			std::string name;
			std::istringstream iss(line);
			iss >> name;
			
			if(name == "@SIZE")
			{
				iss >> xrad >> yrad;
				init(xrad, yrad);
			}
			else if(!name.empty())
			{
				int x, y;
				iss >> x >> y;
				add(name, x, y);
			}
		}
	}
	
	void Map2D::draw(float offs_x, float offs_y)
	{
		propman.draw_props(offs_x, offs_y);
	}
	
	void Map2D::add(std::string prop_type, int x, int y)
	{
		int ptid = PropType::get_by_name(prop_type);
		const PropType& pt = PropType::all[ptid];
		int prop_id = propman.add_prop(prop_type, x, y);
		if(pt.layer == PropType::wall_layer_index)
		{
			for(int i = 0; i < pt.hit_w; ++i)
			{
				for(int j = 0; j < pt.hit_h; ++j)
				{
					set_tile(x + i, y + j, prop_id, ptid);
				}
			}
		}
	}
	
	void Map2D::set_tile(int x, int y, int prop_id, int prop_type)
	{
		x += x_radius;
		y += y_radius;
		if(x >= 0 && x < wall_layer_tiles.width && y >= 0 && y < wall_layer_tiles.height)
		{
			MapTile& t = wall_layer_tiles(x, y);
			t.prop_id = prop_id;
			t.prop_type = prop_type;
		}
	}
	
	MapTile Map2D::get_tile(int x, int y) const
	{
		x += x_radius;
		y += y_radius;
		
		if(x >= 0 && x < wall_layer_tiles.width && y >= 0 && y < wall_layer_tiles.height)
		{
			return wall_layer_tiles(x, y);
		}
		std::cout << "get_tile: out of bounds\n";
		return MapTile();
	}
		
	void Map2D::project_coords(int in_screen_x, int in_screen_y, float& out_x, float& out_y)
	{
		Application* app = &Application::get();
		float sw = app->settings.screen_w, sh = app->settings.screen_h;
		
		float xtiles = sw / tiledim, ytiles = sh / tiledim;
		
		out_x = (float) in_screen_x / sw * xtiles;
		out_y = (float) in_screen_y / sh * ytiles;
		
		out_x -= xtiles * 0.5f;
		out_y = ytiles * 0.5f - out_y;
	}
		
