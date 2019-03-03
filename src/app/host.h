	#ifndef HOST_H
	#define HOST_H
	
	#include "Net.h"
	
	struct Application;
	
	struct Host
	{
		Application* app = 0;
		
		void init();
		
		void handle_msg(const net::Message& msg);

		void handle_hello(const net::Message& msg);
		void handle_ready(const net::Message& msg);
		void handle_client_file(const net::Message& msg);
		void handle_bye(const net::Message& msg);
		
		void handle_input_single(const net::Message& msg);
		
		void send_start_game();
		void send_ready();
		void send_request();
		void send_lobby_data();
		void send_turnfile();
		void send_bye();
		
		void send_input_all();
	};
	
	
	
	#endif
