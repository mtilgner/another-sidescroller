	#include "movement.h"
	#include "animation.h"
	
	#include <iostream>
	
	std::ostream& operator<<(std::ostream& in, GEOM::Vec2 v)
	{
		in << v.x << ", " << v.y;
		return in;
	}
	
	static float epsilon = 0.000001f;
	
	float leftmost(int tile)
	{
		static const float epsilon = 0.00001f;
		return (float) tile + epsilon;
	}
	
	float rightmost(int tile)
	{
		static const float epsilon = 0.00001f;
		static const float one_minus_epsilon = 1.0f - epsilon;
		return (float) tile + one_minus_epsilon;
	}
	
	void to_tile(GEOM::Vec2 pos, int& tile_x, int& tile_y)
	{
		tile_x = pos.x;
		tile_y = pos.y;
		
		if(pos.x != (float) tile_x && pos.x < 0.0f)
		{
			--tile_x;
		}
		if(pos.y != (float) tile_y && pos.y < 0.0f)
		{
			--tile_y;
		}
	}
	
	bool check_ladder(const Map2D& m, int x, int y)
	{
		MapTile t = m.get_tile(x, y);
		if(t.prop_id)
		{
			return PropType::all[t.prop_type].ladder_flag;
		}
		return false;
	}
	
	bool check_ground(GEOM::Vec2 pos, const Map2D& m)
	{
		int tile_x, tile_y;
		to_tile(pos, tile_x, tile_y);
		MapTile t = m.get_tile(tile_x, tile_y - 1);
		return t.prop_id? true : false;
	}
	
	bool blocking_tile(const Map2D& m, int x, int y)
	{
		MapTile t = m.get_tile(x, y);
		return !(t.prop_id == 0 || check_ladder(m, x, y));
	}
	
	CharacterMovement* MovementGrounded::update_position(Character& c, const Map2D& m, int ms)
	{
		using namespace GEOM;
		if(sqlength(c.speed) > 0.0f)
		{
			Vec2 dp = c.speed;
			check_collision_walking(c, dp, m);
//			check_collision_full(c, dp, m);
			c.pos = c.pos + dp;
			int tx, ty;
			to_tile(c.pos, tx, ty);
			
			if(check_ladder(m, tx, ty))
			{
				return c.mov_climbing;
			}
			else if(!check_ground(c.pos, m))
			{
				return c.mov_falling;
			}
		}
		return 0;
	}
	
	CharacterMovement* MovementGrounded::enter_input(Character& c, CharacterInput input, const Map2D& m, int ms)
	{
		using namespace GEOM;
		
		if(!input.action)
		{
			if(c.speed.x != 0.0f)
			{
				c.anim_current = c.facing == CF_RIGHT? c.anim_stand_right : c.anim_stand_left;
				c.anim_current->start(ms);
				
				c.speed = Vec2::null;
			}
		}
		else
		{
			float len = length(input.point);
			if(len > 10.0f)
			{
				len = 10.0f;
			}
			float scale = len / 10.f;
			
			if(input.point.x > 0.0f)
			{
				if(c.speed.x <= 0.0f)
				{
					c.facing = CF_RIGHT;
					c.anim_current = c.anim_walk_right;
					c.anim_current->start(ms);
				}
				c.anim_current->set_speed(scale);
				c.speed = Vec2(scale * x_speed, 0.0f);
			}
			else if(input.point.x < 0.0f)
			{
				if(c.speed.x >= 0.0f)
				{
					c.facing = CF_LEFT;
					c.anim_current = c.anim_walk_left;
					c.anim_current->start(ms);
				}
				c.anim_current->set_speed(scale);
				c.speed = Vec2(-x_speed * scale, 0.0f);
			}
			
			// climb down while standing on ladder
			if(input.point.y < 0.0f)
			{
				int tile_x, tile_y;
				to_tile(c.pos, tile_x, tile_y);
				if(check_ladder(m, tile_x, tile_y - 1))
				{
					Vec2 dp(0.0f, -x_speed);
					if(!check_collision_full(c, dp, m))
					{
						c.speed = dp;
					}
				}
			}
				
		}
		return 0;
	}
	
	void MovementGrounded::on_enter(Character& c, int ms)
	{
		using namespace GEOM;
		
//		std::cout << "enter grounded, pos = " << c.pos << "\n";
		
		c.speed = Vec2::null;
		c.anim_current = c.facing == CF_RIGHT? c.anim_stand_right : c.anim_stand_left;
		c.anim_current->start(ms);
	}
	
	bool MovementGrounded::check_collision_walking(Character& c, GEOM::Vec2& dp, const Map2D& m)
	{
		
		// only checking x-direction
		// later stairs?
		
		using namespace GEOM;
		
		Vec2 np = c.pos + dp;
		int x0, x1, y;
		to_tile(c.pos, x0, y);
		to_tile(np, x1, y);
		if(dp.x < 0.0f)
		{
			for(; x0 >= x1; --x0)
			{
				if(blocking_tile(m, x0, y))
				{
//					std::cout << "direction -, tile found at " << x0 << ", " << y << "\n";
//					std::cout << "new position changed from " << np.x;
					np.x = leftmost(x0 + 1);
//					std::cout << " to " << np.x << "\n";
					dp = np - c.pos;
					return true;
				}
			}
		}
		else
		{
			for(; x0 <= x1; ++x0)
			{
				if(blocking_tile(m, x0, y))
				{
//					std::cout << "direction +, tile found at " << x0 << ", " << y << "\n";
//					std::cout << "new position changed from " << np.x;
					np.x = rightmost(x0 - 1);
//					std::cout << " to " << np.x << "\n";
					dp = np - c.pos;
					return true;
				}
			}
		}
		return false;
	}
	
	
	CharacterMovement* MovementFalling::update_position(Character& c, const Map2D& m, int ms)
	{
		using namespace GEOM;
		
		Vec2 np = c.pos + c.speed;
		
		Vec2 v = np;
		v.y += 1.0f;
		if(check_ground(v, m))
		{
			int tile_x, tile_y;
			to_tile(v, tile_x, tile_y);
			c.pos.y = tile_y;
			c.speed.y = 0.0f;
			
//			std::cout << "falling: found ground, pos: " << np << " -> " << c.pos << "\n";
			
			return c.mov_grounded;
		}
		else
		{
			c.pos = np;
		}
		
		return 0;		
	}
	
	CharacterMovement* MovementFalling::enter_input(Character& c, CharacterInput input, const Map2D& m, int ms)
	{
		if(input.action)
		{
			if(input.point.x < 0.0f && c.facing != CF_LEFT)
			{
				c.facing = CF_LEFT;
				c.anim_current = c.anim_fall_left;
				c.speed.x = -x_speed;
				c.anim_current->start(ms);
			}
			else if(input.point.x > 0.0f && c.facing != CF_RIGHT)
			{
				c.facing = CF_RIGHT;
				c.anim_current = c.anim_fall_right;
				c.speed.x = x_speed;
				c.anim_current->start(ms);
			}
		}
		return 0;
	}
	
	void MovementFalling::on_enter(Character& c, int ms)
	{
		using namespace GEOM;
		
//		std::cout << "enter falling, pos = " << c.pos << "\n";
		
		c.speed = Vec2(0.0f, y_speed);
		c.anim_current = c.facing == CF_RIGHT? c.anim_fall_right : c.anim_fall_left;
		c.anim_current->start(ms);
	}
	
	CharacterMovement* MovementClimbing::update_position(Character& c, const Map2D& m, int ms)
	{
		using namespace GEOM;
		if(sqlength(c.speed) > 0.0f)
		{
			Vec2 dp = c.speed;
			check_collision_full(c, dp, m);
			
			c.pos += dp;
			
			int tx, ty;
			to_tile(c.pos, tx, ty);
			if(!check_ladder(m, tx, ty))
			{
				return c.mov_falling;
			}
		}
		return 0;
	}
	
	CharacterMovement* MovementClimbing::enter_input(Character& c, CharacterInput input, const Map2D& m, int ms)
	{
		using namespace GEOM;
		
		if(input.action)
		{
			c.anim_current = c.anim_climb;
			if(!(input.point.x == 0.0f && input.point.y == 0.0f))
			{
				Vec2 ns = normalize(input.point) * climb_speed;
				if(sqlength(c.speed) == 0.0f)
				{
					c.anim_current->start(ms);
				}
				c.speed = ns;
			}
		}
		else
		{
			c.anim_current = c.anim_climb_still;
			if(sqlength(c.speed) > 0.0f)
			{
				c.anim_current->start(ms);
			}
			c.speed = Vec2::null;
		}
		
		return 0;		
	}
	
	void MovementClimbing::on_enter(Character& c, int ms)
	{
		using namespace GEOM;
		
//		std::cout << "enter climbing\n";
		
		c.speed = Vec2::null;
		c.anim_current = c.anim_climb_still;
		c.anim_current->start(ms);
	}
	
	bool check_collision1(Character& c, GEOM::Vec2& dp, const Map2D& m)
	{
		// direction ++
		
		using namespace GEOM;
		
		bool collision = false;
		
		int x0, x1, y0, y1;
		
		Vec2 topleft = c.pos;
		Vec2 botright = c.pos;
		
		topleft.y += c.hit_box.y;
		topleft.x -= c.hit_box.x * 0.5f;
		botright.x += c.hit_box.x * 0.5f;
		
		Vec2 topright(botright.x, topleft.y);
		
		Vec2 tl = topleft, br = botright, tr = topright;
		
		
		Vec2 step = normalize(dp);
		float len = length(dp);
		int ilen = len;
		float rem = len - ilen;
	
		for(int i = 0; i <= ilen; ++i)
		{
			if(i == ilen)
			{
				step *= rem;
			}
			
			tl += step;
			br += step;
			tr += step;
			
			Vec2 new_tr = tr;
			
			to_tile(tl, x0, y1);
			to_tile(br, x1, y0);
			
			for(int x = x0; x < x1; ++x)
			{
				if(blocking_tile(m, x, y1))
				{
					new_tr.y = (float) y1 - epsilon;
					collision = true;
					break;
				}
			}
			
			for(int y = y0; y < y1; ++y)
			{
				if(blocking_tile(m, x1, y))
				{
					new_tr.x = (float) x1 - epsilon;
					collision = true;
					break;
				}
				
			}
			
			if(blocking_tile(m, x1, y1))
			{
				// check vertical wall
				if(step.x != 0.0f)
				{
					float l = (tr.x - (float) x1) / step.x;
					float mu = tr.y - l * step.y;
					if(mu > (float) y1)
					{
						new_tr.x = (float) x1 - epsilon;
						collision = true;
					}
				}
				// check horizontal wall
				if(step.y != 0.0f)
				{
					float l = (tr.y - (float) y1) / step.y;
					float mu = tr.x - l * step.x;
					if(mu > (float) x1)
					{
						new_tr.y = (float) y1 - epsilon;
						collision = true;
					}
				}	
			}
			if(collision)
			{
//				std::cout << "collision detected, dp: " << dp.x << ", " << dp.y;
				dp = new_tr - topright;
//				std::cout << " -> " << dp.x << ", " << dp.y << "\n";
				return true;
			}
		}
		return false;
	}
	
	bool check_collision2(Character& c, GEOM::Vec2& dp, const Map2D& m)
	{
		// direction +-
		
		using namespace GEOM;
		
		bool collision = false;
		
		int x0, x1, y0, y1;
		
		Vec2 topright = c.pos;
		Vec2 botleft = c.pos;
			
		topright.y += c.hit_box.y;
		topright.x += c.hit_box.x * 0.5f;
		botleft.x -= c.hit_box.x *0.5f;
		
		Vec2 botright(topright.x, botleft.y);
		
		Vec2 bl = botleft, br = botright, tr = topright;
		
		
		Vec2 step = normalize(dp);
		float len = length(dp);
		int ilen = len;
		float rem = len - ilen;
		
		
		
		for(int i = 0; i <= ilen; ++i)
		{
			if(i == ilen)
			{
				step *= rem;
			}
			
			bl += step;
			tr += step;
			br += step;
			
			Vec2 new_br = br;
			
			to_tile(tr, x1, y0);
			to_tile(bl, x0, y1);
			
			for(int x = x0; x < x1; ++x)
			{
				if(blocking_tile(m, x, y1))
				{
					new_br.y = (float) (y1 + 1) + epsilon;
					collision = true;
					break;
				}
			}
			
			for(int y = y0; y > y1; --y)
			{
				if(blocking_tile(m, x1, y))
				{
					new_br.x = (float) x1 - epsilon;
					collision = true;
					break;
				}
				
			}
			
			if(blocking_tile(m, x1, y1))
			{
				// check vertical wall
				if(step.x != 0.0f)
				{
					float l = (br.x - (float) x1) / step.x;
					float mu = br.y - l * step.y;
					if(mu < (float) (y1 + 1))
					{
						new_br.x = (float) x1 - epsilon;
						collision = true;
					}
				}
				// check horizontal wall
				if(step.y != 0.0f)
				{
					float l = (br.y - (float) (y1 + 1)) / step.y;
					float mu = br.x - l * step.x;
					if(mu > (float) x1)
					{
						new_br.y = (float) (y1 + 1) + epsilon;
						collision = true;
					}
				}	
			}
			if(collision)
			{
//				std::cout << "collision detected, dp: " << dp.x << ", " << dp.y;
				dp = new_br - botright;
//				std::cout << " -> " << dp.x << ", " << dp.y << "\n";
				return true;
			}
		}
		return false;
		
	}
	
		
	bool check_collision3(Character& c, GEOM::Vec2& dp, const Map2D& m)
	{
		// direction --
		
		using namespace GEOM;
		
		bool collision = false;
		
		int x0, x1, y0, y1;
		
		Vec2 topleft = c.pos;
		Vec2 botright = c.pos;
			
		topleft.y += c.hit_box.y;
		topleft.x -= c.hit_box.x * 0.5f;
		botright.x += c.hit_box.x *0.5f;
		
		Vec2 botleft(topleft.x, botright.y);
		
		Vec2 bl = botleft, br = botright, tl = topleft;
		
		
		Vec2 step = normalize(dp);
		float len = length(dp);
		int ilen = len;
		float rem = len - ilen;
		
		
		
		for(int i = 0; i <= ilen; ++i)
		{
			if(i == ilen)
			{
				step *= rem;
			}
			
			tl += step;
			br += step;
			bl += step;
			
			Vec2 new_bl = bl;
			
			to_tile(tl, x1, y0);
			to_tile(br, x0, y1);
			
			for(int x = x0; x > x1; --x)
			{
				if(blocking_tile(m, x, y1))
				{
					new_bl.y = (float) (y1 + 1) + epsilon;
					collision = true;
					break;
				}
			}
			
			for(int y = y0; y > y1; --y)
			{
				if(blocking_tile(m, x1, y))
				{
					new_bl.x = (float) (x1 + 1) + epsilon;
					collision = true;
					break;
				}
				
			}
			
			if(blocking_tile(m, x1, y1))
			{
				// check vertical wall
				if(step.x != 0.0f)
				{
					float l = (bl.x - (float) (x1 + 1)) / step.x;
					float mu = bl.y - l * step.y;
					if(mu < (float) (y1 + 1))
					{
						new_bl.x = (float) (x1 + 1) + epsilon;
						collision = true;
					}
				}
				// check horizontal wall
				if(step.y != 0.0f)
				{
					float l = (bl.y - (float) (y1 + 1)) / step.y;
					float mu = bl.x - l * step.x;
					if(mu < (float) (x1 + 1))
					{
						new_bl.y = (float) (y1 + 1) + epsilon;
						collision = true;
					}
				}	
			}
			if(collision)
			{
//				std::cout << "collision detected, dp: " << dp.x << ", " << dp.y;
				dp = new_bl - botleft;
//				std::cout << " -> " << dp.x << ", " << dp.y << "\n";
				return true;
			}
		}
		return false;
	}
	
	bool check_collision4(Character& c, GEOM::Vec2& dp, const Map2D& m)
	{
		// direction -+
		using namespace GEOM;
		
		bool collision = false;
		
		int x0, x1, y0, y1;
		
		Vec2 topright = c.pos;
		Vec2 botleft = c.pos;
			
		topright.y += c.hit_box.y;
		topright.x += c.hit_box.x * 0.5f;
		botleft.x -= c.hit_box.x * 0.5f;
		
		Vec2 topleft(botleft.x, topright.y);
		
		Vec2 bl = botleft, tr = topright, tl = topleft;
		
		
		Vec2 step = normalize(dp);
		float len = length(dp);
		int ilen = len;
		float rem = len - ilen;
		
		
		
		for(int i = 0; i <= ilen; ++i)
		{
			if(i == ilen)
			{
				step *= rem;
			}
			
			tr += step;
			bl += step;
			tl += step;
			
			Vec2 new_tl = tl;
			
			to_tile(bl, x1, y0);
			to_tile(tr, x0, y1);
			
			for(int x = x0; x > x1; --x)
			{
				if(blocking_tile(m, x, y1))
				{
					new_tl.y = (float) y1 - epsilon;
					collision = true;
					break;
				}
			}
			
			for(int y = y0; y < y1; ++y)
			{
				if(blocking_tile(m, x1, y))
				{
					new_tl.x = (float) (x1 + 1) + epsilon;
					collision = true;
					break;
				}
				
			}
			
			if(blocking_tile(m, x1, y1))
			{
				// check vertical wall
				if(step.x != 0.0f)
				{
					float l = (tl.x - (float) (x1 + 1)) / step.x;
					float mu = tl.y - l * step.y;
					if(mu > (float) y1)
					{
						new_tl.x = (float) (x1 + 1) + epsilon;
						collision = true;
					}
				}
				// check horizontal wall
				if(step.y != 0.0f)
				{
					float l = (tl.y - (float) y1) / step.y;
					float mu = tl.x - l * step.x;
					if(mu < (float) (x1 + 1))
					{
						new_tl.y = (float) y1 - epsilon;
						collision = true;
					}
				}	
			}
			if(collision)
			{
//				std::cout << "collision detected, dp: " << dp.x << ", " << dp.y;
				dp = new_tl - topleft;
//				std::cout << " -> " << dp.x << ", " << dp.y << "\n";
				return true;
			}
		}
		return false;
	}
	
	bool check_collision_full(Character& c, GEOM::Vec2& dp, const Map2D& m)
	{
		if(dp.x < 0.0f)
		{
			if(dp.y < 0.0f)
			{
				// --
//				std::cout << "checking --\n";
				return check_collision3(c, dp, m);
			}
			else
			{
				// -+
//				std::cout << "checking -+\n";
				return check_collision4(c, dp, m);
			}
		}
		else
		{
			if(dp.y < 0.0f)
			{
				// +-
//				std::cout << "checking +-\n";
				return check_collision2(c, dp, m);
			}
			else
			{
				// ++
//				std::cout << "checking ++\n";
				return check_collision1(c, dp, m);
			}
		}
		return false;
		
	}
