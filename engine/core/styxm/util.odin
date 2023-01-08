package styxm

v3c_to_v3cf:: proc(col: Vec3c) -> (res: Vec3cf)
{
	res.r = f64(col.r) / 255.0
	res.g = f64(col.g) / 255.0
	res.b = f64(col.b) / 255.0

	return
}
