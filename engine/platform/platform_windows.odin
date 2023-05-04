package platform

import "core:log"
import "core:strings"
import "core:mem"
import "core:os"
import "core:fmt"
import "core:intrinsics"
import win32 "core:sys/windows"

L :: intrinsics.constant_utf16_cstring

foreign import user32 "system:user32.lib"
@(default_calling_convention="stdcall")
foreign user32 {
	SetPropW :: proc(hwnd: win32.HWND, str: win32.LPWSTR, data: win32.HANDLE) -> win32.BOOL ---
	GetPropW :: proc(hwnd: win32.HWND, str: win32.LPWSTR) -> win32.HANDLE ---
}

Window :: struct
{
	title: string,
	width, height: u32,
	running: bool, 

	handle: win32.HWND,
	_native_renderer: D3D11Renderer,
}

_wnd_class: win32.ATOM

init :: proc()
{
	wndclass_desc := win32.WNDCLASSEXW {
		cbSize = size_of(win32.WNDCLASSEXW),
		style = win32.CS_OWNDC | win32.CS_HREDRAW | win32.CS_VREDRAW,
		lpfnWndProc = window_proc,
		lpszClassName = L("Window Class"),
	}
	
	_wnd_class = win32.RegisterClassExW(&wndclass_desc)
	if _wnd_class == 0 {
		log.errorf("Failed to register window class.")
	}
}

quit :: proc()
{

}

window_proc :: proc "stdcall" (hwnd: win32.HWND, 
                               msg: win32.UINT, 
                               wparam: win32.WPARAM, 
                               lparam: win32.LPARAM) -> win32.LRESULT
{
	window := cast(^Window)GetPropW(hwnd, L("Styx"))
	
	switch msg {
		case win32.WM_QUIT: fallthrough
		case win32.WM_DESTROY: fallthrough
		case win32.WM_CLOSE:
			window.running = false
		
		case:
			return win32.DefWindowProcW(hwnd, msg, wparam, lparam)
	}
	
	return 0
}

init_window :: proc(width, height: u32, $title: string) -> (window: ^Window)
{
	window = new(Window)
	
	window.width = width
	window.height = height
	
	window.title = title
	
	window.handle = win32.CreateWindowExW(0,
	                                      L("Window Class"),
	                                      L(title),
	                                      win32.WS_OVERLAPPEDWINDOW | win32.WS_VISIBLE,
	                                      win32.CW_USEDEFAULT, win32.CW_USEDEFAULT,
	                                      i32(width), i32(height),
	                                      nil, nil, nil, nil)

	if window.handle == nil {
		log.errorf("Failed to create window.")
		return
	}
	
	window._native_renderer = init_renderer(window.handle, width, height)
	SetPropW(window.handle, win32.L("Styx"), win32.HANDLE(window))
	
	window.running = true
	
	return
}

free_window :: proc(window: ^Window) 
{
	free_renderer(&window._native_renderer)
	win32.DestroyWindow(window.handle)
	
	free(window)
}

window_process :: proc(using window: ^Window)
{
	msg: win32.MSG
	
	for win32.PeekMessageW(&msg, handle, 0, 0, win32.PM_REMOVE) {
		win32.TranslateMessage(&msg)
		win32.DispatchMessageW(&msg)
	}
}

get_error_string :: proc(err: os.Errno, allocator := context.allocator) -> string
{
	buf: win32.LPWSTR
	err := win32.FormatMessageW(win32.FORMAT_MESSAGE_FROM_SYSTEM,
	                            nil, win32.DWORD(err), 0,
	                            buf, 0, nil)
	if err == 0 do return ""
	defer win32.LocalFree(buf)

	res, _ := win32.wstring_to_utf8(buf, -1, allocator)

	return res
}

list_dir :: proc(dir: string, allocator := context.allocator) -> []File_Entry
{
	handle, err := os.open(dir)
	if err != 0 {
		log.errorf("Failed to create a handle to the directory: %s.", get_error_string(err))
		return nil
	}
	defer os.close(handle)

	dir, err2 := os.read_dir(handle, 0, allocator)
	if err2 != 0 {
		log.errorf("Failed to read_dir: %s.", get_error_string(err))
		return nil
	}

	files := make([]File_Entry, len(dir), allocator)
	for file, i in dir {
		files[i].base_name = file.name
		files[i].abs_path = file.fullpath
	}

	return files
}

// I don't know. I guess if you use context.allocator instead of context.temp_allocator
free_file_entries :: proc(file_entries: []File_Entry)
{
	for file, _ in file_entries {
		delete(file.base_name)
		delete(file.abs_path)
	}
}

change_dir :: proc(path: string)
{
	os.change_directory(path)
}
