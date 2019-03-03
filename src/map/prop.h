	#ifndef PROP_H
	#define PROP_H
	
	#include "proptype.h"
	
	#include <map>
	#include <list>
	
	struct Prop
	{
		Prop(int ptype, int x_pos, int y_pos);
		Prop() = default;
		int type = -1, id = -1;
		int x = 0, y = 0;
	};
	
	class PropManager
	{
	public:
		int add_prop(std::string type, int x, int y);
		void draw_props(float offs_x, float offs_y);
		const Prop& get_prop(int id) const;
	private:
		std::map<int, Prop> props_;
		std::vector<std::map<int, std::list<Prop>>> props_by_layer_;
		Prop invalid_;
	};
		
	
	
	
	
	
	
	
	#endif
