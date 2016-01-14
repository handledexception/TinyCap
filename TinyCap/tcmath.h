#ifndef _H_TCMATH
#define _H_TCMATH

// #include <DirectXMath.h>

// 2D floating-point vector
struct vec2f {
	float x;
	float y;

public:
	vec2f();
	vec2f(float, float);
	~vec2f();
};




// 3D floating-point vector
struct vec3f {
	float x;
	float y;
	float z;

public:
	vec3f();
	vec3f(float, float, float );
	~vec3f();
};





// simple vertex

struct vertex3f {
	vec3f Pos;
};


// simple floating-point rect
struct rectf {
	float left, right, top, bottom;
};

/* struct complexVertex {
	vec3f Pos;
	vec2f Tex;
}; */

#endif