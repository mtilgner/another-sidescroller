	#ifndef CHARACTER_H
	#define CHARACTER_H
	
	#include "geom.h"
	#include "map2d.h"
	
	
	class Animation;
	class CharacterMovement;
	class MovementGrounded;
	class MovementFalling;
	class MovementClimbing;
	
	enum CharacterAction
	{
		CA_NONE = 0,
		CA_MOVE_RIGHT,
		CA_MOVE_LEFT
	};
	
	enum CharacterFacing
	{
		CF_RIGHT,
		CF_LEFT
	};
	
	struct CharacterInput
	{
		int tick = 0;
		GEOM::Vec2 point = GEOM::Vec2::null;
		char action = 0;
	};
	
	struct Character
	{
		Animation
		*anim_stand_right = 0,
		*anim_stand_left = 0,
		*anim_walk_right = 0,
		*anim_walk_left = 0,
		
		*anim_fall_right = 0,
		*anim_fall_left = 0,
		*anim_climb = 0,
		*anim_climb_still = 0
		;
		
		Animation* anim_current = 0;
		
		GEOM::Vec2 pos, speed, hit_box;
		
		MovementGrounded* mov_grounded = 0;
		MovementFalling* mov_falling = 0;
		MovementClimbing* mov_climbing = 0;
		
		CharacterMovement* mov_current = 0;
		
		CharacterInput input;
		
		int facing = CF_RIGHT;
		
		void init_character(const char* file_prefix);
		void clear_character();
		
		void update_position(const Map2D& m, int ms);
		void enter_input(const Map2D& m, CharacterInput newinput, int ms);
		
		void create_animations(const char* file_prefix);
		void free_animations();
		
	};
	
	class CharacterMovement
	{
	public:
		virtual ~CharacterMovement() = default;
		
		virtual CharacterMovement* update_position(Character& c, const Map2D& m, int ms) = 0;
		virtual CharacterMovement* enter_input(Character& c, CharacterInput input, const Map2D& m, int ms) = 0;
		
		virtual void on_enter(Character& c, int ms){}
		
	};

	
	
	#endif
