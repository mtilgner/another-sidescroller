	#include "prop.h"
	#include "shader.h"
	
	#include <iostream>
	
	Prop::Prop(int ptype, int x_pos, int y_pos) : type(ptype), x(x_pos), y(y_pos)
	{}
	
	int get_prop_id()
	{
		static int id_count = 0;
		return ++id_count;
	}
	
	int PropManager::add_prop(std::string type, int x, int y)
	{
		int ptype = PropType::get_by_name(type);
		
		const PropType& pt = PropType::all[ptype];
		Prop p(ptype, x, y);
		p.id = get_prop_id();
		props_[p.id] = p;
		if(pt.layer >= props_by_layer_.size())
		{
			props_by_layer_.resize(pt.layer + 1);
		}
		
		auto& L = props_by_layer_[pt.layer][pt.id];
		
		L.push_back(p);
		
		return p.id;
	}
	
	void PropManager::draw_props(float offs_x, float offs_y)
	{
		for(int i = 0; i < props_by_layer_.size(); ++i)
		{
			auto& m = props_by_layer_[i];
			float paralax = PropType::layers[i].paralax;
			for(auto& p : m)
			{
				PropType& pt = PropType::all[p.first];
				select_texture(pt.tex);
				for(Prop& prop : p.second)
				{
					draw_vbo(pt.vbo, paralax * offs_x + (float) prop.x, offs_y + (float) prop.y);
				}
			}
		}
	}
	
	const Prop& PropManager::get_prop(int id) const
	{
		auto it = props_.find(id);
		if(it != props_.end())
		{
			return it->second;
		}
		std::cout << "Prop " << id << " not found\n";
		return invalid_;
	}
