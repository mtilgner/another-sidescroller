	#ifndef RAW_DATA_H
	#define RAW_DATA_H
	
	#include <vector>
	#include <string>
	
	struct RawData
	{
		std::string file;
		std::vector<char> data;
	};
	
	RawData load_raw_data(std::string file);
	void save_raw_data(const RawData& raw);
		
	
	
	
	
	#endif
