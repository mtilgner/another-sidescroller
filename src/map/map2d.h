	#ifndef MAP2D_H
	#define MAP2D_H
	
	#include "prop.h"
	#include "grid.h"
	
	#include <map>
	
	struct MapTile
	{
		int prop_id = 0, prop_type = -1;
	};
	
	struct Map2D
	{
		PropManager propman;
		float tiledim = 20.0f;
		Grid<MapTile> wall_layer_tiles;
		int x_radius = 0, y_radius = 0;
		
		void init(int xrad, int yrad);
		
		void load(std::string map_file);
		
		void draw(float offs_x, float offs_y);
		void update(int ms);
		void add(std::string prop_type, int x, int y);
		
		void set_tile(int x, int y, int prop_id, int prop_type);
		MapTile get_tile(int x, int y) const;
		
		void project_coords(int in_screen_x, int in_screen_y, float& out_x, float& out_y);
	};
	
	
	#endif
