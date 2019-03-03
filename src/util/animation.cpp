	#include "animation.h"
	#include "shader.h"
	
	void Animation::set_speed(float s)
	{
		speed_ = s;
	}
	
	void Animation::set_loop(bool l)
	{
		loop_ = l;
	}
	
	
	
	
	
	AnimCharacter::~AnimCharacter()
	{
		glDeleteBuffers(1, &vbo_);
	}
	
	void AnimCharacter::start(int ms)
	{
		last_update_ = ms;
		frame_ = 0;
	}
	
	void AnimCharacter::draw(float screen_x, float screen_y)
	{
		draw_vbo_texture(vbo_, tex_[frame_], screen_x, screen_y);
	}
	
	void AnimCharacter::update(int ms)
	{
		int dt = ms - last_update_;
		
		int inter = speed_ > 0.0f? (float) interval_ / speed_ : interval_;
		
		if(dt >= inter)
		{
			while(dt >= inter)
			{
				++frame_;
				dt -= inter;
			}
			frame_ = frame_ % num_frames_;
			last_update_ = ms - dt;
		}
	}
	
	void AnimCharacter::set_interval(int interval)
	{
		interval_ = interval;
	}
	
	void AnimCharacter::set_vbo(GLuint vbo)
	{
		vbo_ = vbo;
	}
	
	void AnimCharacter::add_frame(GLuint texture)
	{
		tex_.push_back(texture);
		num_frames_ = tex_.size();
	}
		
