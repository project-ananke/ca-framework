// Courtesy of Stygatore

// Vec4 -> f32: vec4f
typedef struct vec4f vec4f;
struct vec4f
{
	f32 x;
	f32 y;
	f32 z;
	f32 w;
};

// Vec4 -> u8: vec4c
typedef struct vec4c vec4c;
struct vec4c
{
	u8 x;
	u8 y;
	u8 z;
	u8 w;
};

// Vec3 -> f32, vec3f_lit: vec3f
typedef union vec3f vec3f;
union vec3f
{
	struct {
		f32 x;
		f32 y;
		f32 z;
	};
	struct {
		f32 r;
		f32 g;
		f32 b;
	};
};

#define vec3f_lit(x1, y1, z1) ((vec3f){.x = x1, .y = y1, .z = z1})

// Vec3 -> u8, vec3c_lit: vec3c
typedef union vec3c vec3c;
union vec3c
{
	struct {
		u8 x;
		u8 y;
		u8 z;
	};
	struct {
		u8 r;
		u8 g;
		u8 b;
	};
};

#define vec3c_lit(x1, y1, z1) ((vec3c){.x = x1, .y = y1, .z = z1})

// Vec2 -> f32, vec2f_lit: vec2f
typedef struct vec2f vec2f;
struct vec2f
{
	f32 x;
	f32 y;
};

#define vec2f_lit(x1, y1) ((vec2f){.x = x1, .y = y1})

// Vec2 -> u32, vec2c_lit: vec2_coords
typedef struct vec2_coords vec2_coords;
struct vec2_coords
{
	u32 x;
	u32 y;
};

#define vec2c_lit(x1, y1) ((vec2_coords){.x = x1, .y = y1})

