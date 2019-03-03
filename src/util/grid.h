	#ifndef GRID_H
	#define GRID_H
	
	#include <vector>
	
	template<class T>
	struct Grid
	{
		std::vector<T> data;
		unsigned width = 0, height = 0;
		
		Grid() = default;
		
		Grid(unsigned w, unsigned h) : width(w), height(h)
		{
			data.resize(w * h);
		}
		
		void resize(unsigned w, unsigned h)
		{
			width = w;
			height = h;
			data.resize(w * h);
		}
		
		T& operator()(unsigned x, unsigned y)
		{
			return data[x * width + y];
		}
		
		const T& operator()(unsigned x, unsigned y) const
		{
			return data[x * width + y];
		}
	};
		
		
	
	
	
	#endif
