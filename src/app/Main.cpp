	#include "application.h"

	
	
	int main()
	{	
		Application& app = Application::get();
		
		app.init();
		app.loop();
		app.exit();

		return 0;
	}

	
