	#ifndef ANIMATION_H
	#define ANIMATION_H
	
	#include <vector>
	#include <GL/glew.h>
	
	class Animation
	{
	public:
		virtual ~Animation() = default;
		
		virtual void start(int ms){}
		virtual void draw(float screen_x, float screen_y){}
		virtual void update(int ms){}
		void set_speed(float s);
		void set_loop(bool l);
		
	protected:
		float speed_ = 1.0f;
		bool loop_ = true;
	};
	
	class AnimCharacter : public Animation
	{
	public:
		~AnimCharacter();
		
		void start(int ms);
		void draw(float screen_x, float screen_y);
		void update(int ms);
		
		void set_interval(int interval);
		void set_vbo(GLuint vbo);
		void add_frame(GLuint texture);
		
	private:
		int interval_ = 0, last_update_ = 0, frame_ = 0, num_frames_ = 0;
		GLuint vbo_ = 0;
		std::vector<GLuint> tex_;
	};
	
	#endif
