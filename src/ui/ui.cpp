	#include "ui.h"
	#include "application.h"
	
	void UI::init(TTF_Font* ttf_font, SDL_Renderer* renderer)
	{
		using namespace tab;
		
		//SDLFont::init(renderer);
		font.load(ttf_font);
		
		Table::default_font = &font;
	}
	
	void UI::quit()
	{
		font.clear();
	}
	
	void cell_draw(tab::Cell* c, SDL_Rect r);
	
	void cb_quit()
	{
		Application::get().quit();
	}
	
	void make_button(tab::Cell& c)
	{
		using namespace tab;
		
//		c.render = &cell_draw;
		tab::TextAttributes attr;
		attr.r = 120;
		attr.g = 120;
		attr.b = 120;
		attr.a = 255;
		
		c.text_attr = attr;
		c.align = ALIGN_CENTER;
		c.event_filter = EVENT_CLICK_LEFT;
	}
	
	
	void cell_draw(tab::Cell* c, SDL_Rect r)
	{
		tab::Font* f = tab::Table::default_font;
		
		f->color(0, 50, 100);
		f->draw_rect(r);
		r.x += 2;
		r.y += 2;
		r.w -= 4;
		r.h -= 4;
		if(c->is_highlighted())
		{
			f->color(50, 50, 50);
		}
		else
		{
			f->color(0, 0, 0);
		}
		f->draw_rect(r);
	}
		
		
