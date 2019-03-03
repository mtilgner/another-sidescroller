	#include "ui_log.h"
	#include "application.h"
	
	void UILog::init()
	{
		using namespace tab;
		
		app = &Application::get();
		
		TableAttributes ta;
		SDL_Rect rect = {500, 50, 300, 200};
		ta.rect = rect;
		ta.visible_rows = 8;
		ta.cell_border = 5;
		ta.table_border = 5;
		
		log_table.set_attributes(ta);
		log_table.show(false);
	}
	
	void UILog::load_ui()
	{
		log_table.show();
	}
	
	void UILog::unload_ui()
	{
		log_table.show(false);
	}
	
	void UILog::print(std::string str)
	{
		using namespace tab;
		
		Row r = log_table.add_row();
		r[0].text = str;
		
		log_table.scroll_to_end();
	}
