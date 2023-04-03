package styxm

v3c_to_v3cf :: proc(col: Vec3c) -> (res: Vec3f)
{
	res.r = f32(col.r) / 255.0
	res.g = f32(col.g) / 255.0
	res.b = f32(col.b) / 255.0

	return
}

v4c_to_v4f :: proc(col: Vec4c) -> (res: Vec4f)
{
	res.r = f32(col.r) / 255.0
	res.g = f32(col.g) / 255.0
	res.b = f32(col.b) / 255.0
	res.a = f32(col.a) / 255.0

	return
}

_v3co_to_v3f_bundled :: proc(v3coord: Vec3Coord) -> (res: Vec3f)
{
	res.x = f32(v3coord.x)
	res.y = f32(v3coord.y)
	res.z = f32(v3coord.z)

	return
}
_v3co_to_v3f_raw :: proc(x, y, z: i32) -> (res: Vec3f)
{
	res.x = f32(x)
	res.y = f32(y)
	res.z = f32(z)

	return
}

v3co_to_v3f :: proc{_v3co_to_v3f_bundled, _v3co_to_v3f_raw}

// NOTE(sir->w7): This is a very scuffed orthographic projection,
// and it will not be reliable for any 3D space rendering.
screen2d_orthoproj :: proc(w, h: f32) -> matrix[4, 4]f32
{
	return matrix[4, 4]f32{
		2.0/w,   0.0,     0.0,       -1.0,
		0.0,    -2.0/h,   0.0,       1.0, 
		0.0,     0.0,     1.0/1000,   0.0, 
	    0.0,     0.0,     0.0,        1.0,
	}
}