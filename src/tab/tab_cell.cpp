	
	#ifndef MATE_COMPILE
	#include "tab_cell.h"
	#include "tab_table.h"
	#endif

	namespace tab
	{
		void Cell::init(Table* table, CellType type, unsigned tabnum, unsigned colnum, unsigned rownum)
		{
			table_ = table;
			type_ = type;
			tab_ = tabnum;
			col_ = colnum;
			row_ = rownum;
		}
		
		SDL_Rect Cell::rect() const
		{
			return table_->rect(this);
		}
		
		bool Cell::is_highlighted() const
		{
			return table_->is_highlighted(*this);
		}
		
		
	} // namespace tab