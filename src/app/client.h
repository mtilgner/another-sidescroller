	#ifndef CLIENT_H
	#define CLIENT_H
	
	#include "Net.h"
	
	struct Application;
	
	struct Client
	{
		Application* app = 0;
		
		void init();
		
		void handle_msg(const net::Message& msg);
		
		void handle_start_game(const net::Message& msg);
		void handle_request(const net::Message& msg);
		void handle_lobby_data(const net::Message& msg);
		void handle_turnfile(const net::Message& msg);
		void handle_bye(const net::Message& msg);
		
		void handle_input_all(const net::Message& msg);
		
		void send_hello();
		void send_ready();
		void send_client_file();
		void send_bye();
		
		void send_input_single();
	};
	
	
	
	
	
	
	#endif
