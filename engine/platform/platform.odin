package platform

import "core:time"

File_Entry :: struct
{
	base_name: string,
	abs_path: string,
}

cap_fps :: proc(cap: u32)
{
	@static time_last: time.Tick
	if time_last._nsec == 0 {
		time_last = time.tick_now()
		return
	}

	d := time.tick_since(time_last)
	d_ms := time.duration_milliseconds(d)

	ms_cap := 1000.0 / f64(cap)
	if d_ms < ms_cap {
		// One millisecond is 1/1000 of a second.
		// One nanosecond is 1/1000000000 of a second. 
		time.sleep(time.Duration((ms_cap - d_ms) * 1000000.0))
	}

	time_last = time.tick_now()
}