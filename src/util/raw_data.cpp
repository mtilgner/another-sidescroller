	#include "raw_data.h"
	
	#include <fstream>
	
	
	RawData load_raw_data(std::string file)
	{
		RawData raw;
		raw.file = file;
		std::ifstream in(file);
		std::istreambuf_iterator<char> it1(in), it2;
		raw.data = std::vector<char>(it1, it2);
		
		return raw;
	}
	
	void save_raw_data(const RawData& raw)
	{
		std::ofstream out(raw.file, std::ofstream::binary);
		out.write(&raw.data.front(), raw.data.size());
	}
