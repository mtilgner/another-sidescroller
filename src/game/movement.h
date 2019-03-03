	#ifndef MOVEMENT_H
	#define MOVEMENT_H
	
	#include "character.h"
	
	
	class MovementGrounded : public CharacterMovement
	{
	public:
		
		CharacterMovement* update_position(Character& c, const Map2D& m, int ms);
		CharacterMovement* enter_input(Character& c, CharacterInput input, const Map2D& m, int ms);
		void on_enter(Character& c, int ms);
		
		bool check_collision_walking(Character& c, GEOM::Vec2& dp, const Map2D& m);
		
		float x_speed = 0.5f;
		
	};
	
	class MovementFalling : public CharacterMovement
	{
	public:
		CharacterMovement* update_position(Character& c, const Map2D& m, int ms);
		CharacterMovement* enter_input(Character& c, CharacterInput input, const Map2D& m, int ms);
		void on_enter(Character& c, int ms);
		
		float x_speed = 0.2f, y_speed = -0.9f;
	
	};
	
	class MovementClimbing : public CharacterMovement
	{
	public:
	
		CharacterMovement* update_position(Character& c, const Map2D& m, int ms);
		CharacterMovement* enter_input(Character& c, CharacterInput input, const Map2D& m, int ms);
		void on_enter(Character& c, int ms);
		
		float climb_speed = 0.5f;
	};
	
	bool check_ground(GEOM::Vec2 pos, const Map2D& m);
	bool check_collision_full(Character& c, GEOM::Vec2& dp, const Map2D& m);
	
	
	#endif
