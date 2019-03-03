	#include "net_message.h"
	
	std::string read_string(net::MessageReader& rd)
	{
		std::string str;
		size_t s = 0;
		
		rd.read(s);
		str.resize(s);
		for(char& c : str)
		{
			rd.read(c);
		}
		return str;
	}
	
	void write_string(net::MessageWriter& wr, std::string str)
	{
		wr.write(str.size());
		for(char c : str)
		{
			wr.write(c);
		}
	}
		
