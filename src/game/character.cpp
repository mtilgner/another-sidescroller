	#include "character.h"
	#include "texture.h"
	#include "animation.h"
	#include "shader.h"
	#include "movement.h"
	
	#include <sstream>
	#include <iostream>
	
	void Character::init_character(const char* file_prefix)
	{
		create_animations(file_prefix);
		mov_grounded = new MovementGrounded;
		mov_falling = new MovementFalling;
		mov_climbing = new MovementClimbing;
		
		mov_current = mov_grounded;
	}
	
	void Character::clear_character()
	{
		free_animations();
		delete mov_grounded;
		delete mov_falling;
		delete mov_climbing;
		
		mov_grounded = 0;
		mov_falling = 0;
		mov_climbing = 0;
		mov_current = 0;
	}
	
	void Character::update_position(const Map2D& m, int ms)
	{
		CharacterMovement* mov = mov_current->update_position(*this, m, ms);
		if(mov)
		{
			mov->on_enter(*this, ms);
			mov_current = mov;
		}
	}
	
	void Character::enter_input(const Map2D& m, CharacterInput newinput, int ms)
	{
		CharacterMovement* mov = mov_current->enter_input(*this, newinput, m, ms);
		if(mov)
		{
			mov->on_enter(*this, ms);
			mov_current = mov;
		}
	}
	
	void Character::create_animations(const char* file_prefix)
	{
		TextureManager& tm = TextureManager::get();
		
		AnimCharacter* sr = new AnimCharacter;
        AnimCharacter* sl = new AnimCharacter;
        AnimCharacter* wr = new AnimCharacter;
        AnimCharacter* wl = new AnimCharacter;
        
        AnimCharacter* fr = new AnimCharacter;
        AnimCharacter* fl = new AnimCharacter;
        AnimCharacter* cl = new AnimCharacter;
        AnimCharacter* cs = new AnimCharacter;
        
        std::string t0 = file_prefix;
        t0 = t0 + "0";
        
        GLuint tex0 = tm.texture_load(t0.c_str());
        
        float w = 1.0f * 2.0f, h = 1.56 * 2.0f;
        
        hit_box.x = w;
        hit_box.y = h;
        
        sr->set_vbo(gen_vbo_simple(w, h, -w * 0.5f));
        wr->set_vbo(gen_vbo_simple(w, h, -w * 0.5f));
        sl->set_vbo(gen_vbo_simple_mirrored(w, h, -w * 0.5f));
        wl->set_vbo(gen_vbo_simple_mirrored(w, h, -w * 0.5f));
        
        fr->set_vbo(gen_vbo_simple(w, h, -w * 0.5f));
        fl->set_vbo(gen_vbo_simple_mirrored(w, h, -w * 0.5f));
        cl->set_vbo(gen_vbo_simple(w, h, -w * 0.5f));
        cs->set_vbo(gen_vbo_simple(w, h, -w * 0.5f));
        
        sr->add_frame(tex0);
        sl->add_frame(tex0);
        
        // dirty
        GLuint t6 = tm.texture_load("pixelman/p6");
        fr->add_frame(t6);
        fl->add_frame(t6);
        cs->add_frame(tm.texture_load("pixelman/c0"));
        
        for(int i = 0; i < 4; ++i)
        {
			std::ostringstream oss;
			oss << "pixelman/c" << i;
			GLuint tex = tm.texture_load(oss.str().c_str());
			cl->add_frame(tex);
		}
        
        for(int i = 1; i < 7; ++i)
        {
			std::ostringstream oss;
			oss << file_prefix << i;
			GLuint tex = tm.texture_load(oss.str().c_str());
			wr->add_frame(tex);
			wl->add_frame(tex);
		}
		
		sr->set_interval(80);
		sl->set_interval(80);
		wr->set_interval(80);
		wl->set_interval(80);
		
		fr->set_interval(80);
		fl->set_interval(80);
		cl->set_interval(120);
		cs->set_interval(80);
		
		
		anim_stand_left = sl;
		anim_stand_right = sr;
		anim_walk_left = wl;
		anim_walk_right = wr;
		
		anim_fall_left = fl;
		anim_fall_right = fr;
		anim_climb = cl;
		anim_climb_still = cs;
		
		anim_current = anim_stand_right;	
	}
	
	
	void Character::free_animations()
	{
		delete anim_stand_left;
		anim_stand_left = 0;
		
		delete anim_stand_right;
		anim_stand_right = 0;
		
		delete anim_walk_left;
		anim_walk_left = 0;
		
		delete anim_walk_right;
		anim_walk_right = 0;
		
		anim_current = 0;
	}
						
						
				
