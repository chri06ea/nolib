#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define OPENGL
#pragma comment(lib, "opengl32.lib")

//////////////////////////////////////////////////////////////////////
// Default values

#define DEFAULT_WINDOW_WIDTH  1024
#define DEFAULT_WINDOW_HEIGHT 768

//////////////////////////////////////////////////////////////////////
// Core types

#define null 0
#define ch8 char
#define ch16 wchar_t
#define u8 unsigned __int8
#define u32 unsigned __int32
#define u64 unsigned __int64
#define i8 __int8
#define i16 __int16
#define i32 __int32
#define i64 __int64
#define f32 float
#define f64 double

#define intptr int* // TODO: platform_intptr maybe?
#define uintptr unsigned int*

#define str char *

#define bool u8

#define true 1
#define false 0

typedef struct {
	f32 x, y;
} v2f;

typedef struct {
	f32 x, y, z;
} v3f;

typedef struct {
	f32 r, g, b;
} c3f;

typedef struct {
	f32 r, g, b, a;
} c4f;

const c4f C4F_WHITE = {
	1.f, 1.f, 1.f, 1.f
};
const c4f C4F_RED = {
	1.f, 0.f, 0.f, 1.f
};
const c4f C4F_GREEN = {
	0.f, 1.f, 0.f, 1.f
};
const c4f C4F_BLUE = {
	0.f, 0.f, 1.f, 1.f
};

typedef struct {
	f32 x, y, z, w;
} v4f;

typedef struct {
	f32 w, h;
} s2f;

//////////////////////////////////////////////////////////////////////
// Core helper macros

#define fail_if_false(x) assert(x)

#define trace_information(fmt, ...) printf(fmt, __VA_ARGS__);

//////////////////////////////////////////////////////////////////////
// Math

inline v2f add_v2f(const v2f* v, f32 x, f32 y)
{
	v2f r;
	r.x = v->x + x;
	r.y = v->y + y;
	return r;
}

inline v2f add_v2f_v2f(const v2f* v, const v2f* v2)
{
	v2f r;
	r.x = v->x + v2->x;
	r.y = v->y + v2->y;
	return r;
}

//////////////////////////////////////////////////////////////////////
// Allocation

void* alloc_buffer(u32 size)
{
	void* allocated = malloc(size);
	return allocated;
}

void free_buffer(void* buffer)
{
	free(buffer);
}

//////////////////////////////////////////////////////////////////////
// Core Utils

ch8* str_wstr_to_str(ch16* widestr)
{
	size_t len = wcslen(widestr);
	char* s = (char*) malloc((len + 1) * sizeof(char));
	if(!s) {
		return NULL;
	}
	size_t converted = 0;
	wcstombs_s(&converted, s, len + 1, widestr, _TRUNCATE);
	return s;
}

ch8* str_concat(ch8* s1, ch8* s2)
{
	u32 buf_len = (u32) strlen(s1) + (u32) strlen(s2) + 1;
	char* result = alloc_buffer(buf_len);
	strcpy_s(result, buf_len, s1);
	strcat_s(result, buf_len, result);
	return result;
}

u32 str_length(ch8* s)
{
	u32 length = 0;

	while(s[length] != '\0') length++;

	return length;
}

//////////////////////////////////////////////////////////////////////
// Filesystem


//////////////////////////////////////////////////////////////////////
// Memory area

//typedef struct {
//	u8* base;
//	u32 size_used, size;
//} MemoryArea;
//
//void init_memory_area(MemoryArea* memory_area, u32 size)
//{
//	memory_area->base = (u8*) alloc_buffer(size);
//	memory_area->size = 0;
//	memory_area->size_used = 0;
//}
//
//void* write_memory_area(MemoryArea* memory_area, void* buffer, u32 buffer_size)
//{
//	void* area_base = memory_area->base + memory_area->size_used;
//	memory_area->size_used += buffer_size;
//	fail_if_false(memory_area->size_used > memory_area->size);
//	return area_base;
//}

//////////////////////////////////////////////////////////////////////
// Resource

typedef struct {
	void* buffer;
	u32 buffer_size;
	const ch8* path;
	u32 size;
	u32 last_write_time;
	u32 resource_open_fail_count;
} Resource;

Resource _loaded_resources[50];

bool resource_create_or_update_from_path(Resource* resource)
{
	#ifdef WIN32
	HANDLE file_handle;

	file_handle = CreateFileA(resource->path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	fail_if_false(file_handle != NULL);

	if(file_handle == INVALID_HANDLE_VALUE)
	{
		resource->resource_open_fail_count++;
		if(resource->resource_open_fail_count > 10)
		{
			trace_information("Failed opening resource %s\n", resource->path);
		}

		return false;
	}

	resource->resource_open_fail_count = 0;

	FILETIME ftCreationTime, ftLastAccessTime, ftLastWriteTime;

	fail_if_false(GetFileTime(file_handle, &ftCreationTime, &ftLastAccessTime, &ftLastWriteTime));

	if(resource->last_write_time != ftLastWriteTime.dwLowDateTime)
	{
		trace_information("Updating resource %s\n", resource->path);

		DWORD file_size_low, file_size_high, file_size_read;

		file_size_low = GetFileSize(file_handle, &file_size_high);
		fail_if_false(file_size_low && !file_size_high);

		if(resource->buffer)
			free_buffer(resource->buffer);

		resource->buffer = alloc_buffer(file_size_low + 1);
		resource->buffer_size = file_size_low + 1;

		memset(resource->buffer, 0, file_size_low + 1);

		fail_if_false(ReadFile(file_handle, resource->buffer, file_size_low, &file_size_read, NULL));
		fail_if_false(file_size_read == file_size_low);

		resource->last_write_time = ftLastWriteTime.dwLowDateTime;
		resource->size = file_size_read;

		CloseHandle(file_handle);

		return true;
	}

	CloseHandle(file_handle);

	return false;

	#endif
}

void resource_init_from_path(const ch8* path, Resource* resource)
{
	resource->buffer = 0;
	resource->buffer_size = 0;
	resource->path = path;
	resource->size = 0;
	resource->last_write_time = 0;
	resource->resource_open_fail_count = 0;

	fail_if_false(resource_create_or_update_from_path(resource));
}

bool resource_update(Resource* resource)
{
	return resource_create_or_update_from_path(resource);
}

void resource_init_from_folder(const ch8* path, Resource* resources, u32* resource_count, u32 resources_max_count)
{
	char find_fild_path[MAX_PATH];
	sprintf_s(find_fild_path, sizeof(find_fild_path), "%s/*", path);

	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA(find_fild_path, &fd);

	*resource_count = 0;

	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				ch8* file_path = alloc_buffer(MAX_PATH);
				sprintf_s(file_path, MAX_PATH, "%s/%s", path, fd.cFileName);

				resource_init_from_path(file_path, &resources[*resource_count]);

				(*resource_count)++;
			}
		} while(FindNextFileA(hFind, &fd));

		FindClose(hFind);
	}
}

//////////////////////////////////////////////////////////////////////
// WGL

#ifdef _WIN32

#pragma region

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001          
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_TYPE_RGBA_ARB 0x202B

HGLRC(__stdcall* wglCreateContextAttribsARB)(HDC hdc, HGLRC hShareContext, const i32* attribList);
bool(__stdcall* wglChoosePixelFormatARB)(HDC hdc, const i32* piAttribIList, const f32* pfAttribFList, u32 nMaxFormats, i32* piFormats, u32* nNumFormats);
void* (__stdcall* wglGetProcAddress2)(str name);
void* (__stdcall* wglSwapIntervalEXT)(u8 vsync);

#pragma endregion

void* wgl_get_proc_address(HMODULE module, str name)
{
	void* result = 0;

	result = wglGetProcAddress(name);

	if(result)
		return result;

	return GetProcAddress(module, name);
}

void wgl_init_opengl()
{
	HMODULE opengl_module;
	HWND dummy_window_handle;
	HDC dummy_device_context;
	u32 dummy_pixel_format;
	HGLRC dummy_rendering_context;
	HMODULE module_handle;

	fail_if_false(module_handle = GetModuleHandle(NULL));

	#define _WGL_DUMMY_WINDOW_CLASS_NAME "_dummy_window_class_name"

	WNDCLASSEXA dummy_window_class = {
		.cbSize = sizeof(WNDCLASSEXA),
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc = DefWindowProcA,
		.hInstance = module_handle,
		.hCursor = LoadCursor(0, IDC_ARROW),
		.lpszClassName = _WGL_DUMMY_WINDOW_CLASS_NAME
	};

	PIXELFORMATDESCRIPTOR dummy_pixel_format_desc = {
		.nSize = sizeof(PIXELFORMATDESCRIPTOR),
		.nVersion = 1,
		.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType = PFD_TYPE_RGBA,
		.cColorBits = 32,
		.cAlphaBits = 8,
		.cDepthBits = 24,
		.cStencilBits = 8,
		.iLayerType = PFD_MAIN_PLANE,
	};

	fail_if_false(opengl_module = LoadLibraryA("opengl32.dll"));

	fail_if_false(RegisterClassExA(&dummy_window_class));

	fail_if_false(dummy_window_handle = CreateWindowExA(0, _WGL_DUMMY_WINDOW_CLASS_NAME, "_dummy_window_title_name",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, 0, module_handle, 0));

	fail_if_false(dummy_device_context = GetDC(dummy_window_handle));

	fail_if_false(dummy_pixel_format = ChoosePixelFormat(dummy_device_context, &dummy_pixel_format_desc));

	fail_if_false(SetPixelFormat(dummy_device_context, dummy_pixel_format, &dummy_pixel_format_desc));

	fail_if_false(dummy_rendering_context = wglCreateContext(dummy_device_context));

	fail_if_false(wglMakeCurrent(dummy_device_context, dummy_rendering_context));

	fail_if_false(wglChoosePixelFormatARB = wgl_get_proc_address(opengl_module, "wglChoosePixelFormatARB"));
	fail_if_false(wglCreateContextAttribsARB = wgl_get_proc_address(opengl_module, "wglCreateContextAttribsARB"));
	fail_if_false(wglSwapIntervalEXT = wgl_get_proc_address(opengl_module, "wglSwapIntervalEXT"));

	fail_if_false(wglMakeCurrent(dummy_device_context, 0));

	fail_if_false(wglDeleteContext(dummy_rendering_context));

	fail_if_false(ReleaseDC(dummy_window_handle, dummy_device_context));

	fail_if_false(DestroyWindow(dummy_window_handle));
}

#endif

//////////////////////////////////////////////////////////////////////
// OpenGL

#pragma region

#define MAX_SHADER_ATTRIBUTES 15

typedef struct {
	u32 type;
	u32 count;
} ShaderAttribute;

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_QUADS 0x0007
#define GL_UNSIGNED_INT 0x1405
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_2D 0x0DE1
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_REPEAT 0x2901
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_COLOR 0x1800
#define GL_DEPTH 0x1801
#define GL_STENCIL 0x1802
#define GL_STENCIL_INDEX 0x1901
#define GL_DEPTH_COMPONENT 0x1902
#define GL_RED 0x1903
#define GL_GREEN 0x1904
#define GL_BLUE 0x1905
#define GL_ALPHA 0x1906
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_POINT 0x1B00
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_KEEP 0x1E00
#define GL_REPLACE 0x1E01
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_BLEND_DST 0x0BE0
#define GL_BLEND_SRC 0x0BE1
#define GL_BLEND 0x0BE2
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_SRGB 0x8C40
#define GL_SRGB8 0x8C41
#define GL_SRGB_ALPHA 0x8C42
#define GL_SRGB8_ALPHA8 0x8C43

void(__stdcall* glBindBuffer)(u32 target, u32 buffer);
void(__stdcall* glGenBuffers)(i32 count, u32* buffers);
void(__stdcall* glBufferData)(u32 target, size_t sprite_size, const void* data, u32 usage);
void(__stdcall* glBindBufferBase)(u32 target, u32 index, u32 buffer);
void(__stdcall* glBufferSubData)(u32 target, intptr offset, uintptr sprite_size, const void* data);
void(__stdcall* glGetShaderiv)(u32 program, u32 pname, int* params);
u32(__stdcall* glCreateShader)(u32 type);
void(__stdcall* glShaderSource)(u32 _shader, i32 count, const char* const* string, const int* length);
void(__stdcall* glCompileShader)(u32 _shader);
u32(__stdcall* glCreateProgram)(void);
void(__stdcall* glAttachShader)(u32 program, u32 _shader);
void(__stdcall* glLinkProgram)(u32 program);
void(__stdcall* glGetProgramiv)(u32 program, u32 pname, int* params);
void(__stdcall* glGetProgramInfoLog)(u32 program, i32 bufSize, i32* length, char* infoLog);
void(__stdcall* glGetShaderInfoLog)(u32 _shader, i32 bufSize, i32* length, char* infoLog);
void(__stdcall* glDeleteShader)(u32 _shader);
void(__stdcall* glEnableVertexAttribArray)(u32 index);
void(__stdcall* glVertexAttribPointer)(u32 index, i32 sprite_size, u32 type, u8 normalized, i32 stride, const void* pointer);
void(__stdcall* glGenTextures)(i32 n, u32* textures);
void(__stdcall* glBindTexture)(u32 target, u32 _atlas_texture);
void(__stdcall* glTexParameteri)(u32 target, u32 pname, u32 params);
void(__stdcall* glTexImage2D)(u32 target, i32 level, i32 internalformat, i32 sprite_width, i32 height, i32 border, u32 format, u32 type, const void* pixels);
void(__stdcall* glGenerateMipmap)(u32 target);
void(__stdcall* glDrawElements)(u32 mode, i32 count, u32 type, const void* indices);
void(__stdcall* glViewport)(i32 x, i32 y, i32 sprite_width, i32 height);
void(__stdcall* glClearColor)(f32 red, f32 green, f32 blue, f32 alpha);
void(__stdcall* glClear)(u32 mask);
void(__stdcall* glUseProgram)(u32 program);
void(__stdcall* glTranslatef)(f32 x, f32 y, f32 z);
void(__stdcall* glScalef)(f32 x, f32 y, f32 z);
void(__stdcall* glRotatef)(f32 angle, f32 x, f32 y, f32 z);
void(__stdcall* glMatrixMode)(u32 mode);
void(__stdcall* glLoadIdentity)();
void(__stdcall* glRotatef)(f32 angle, f32 x, f32 y, f32 z);
void(__stdcall* glOrtho)(f64 left, f64 right, f64 bottom, f64 top, f64 zNear, f64 zFar);
u32(__stdcall* glGetError)();
void* (__stdcall* glMapBuffer)(u32 target, u32 access);
u8(__stdcall* glUnmapBuffer)(u32 target);
u8(__stdcall* glEnable)(u32 a1);
u8(__stdcall* glDisable)(u32 a1);
u8(__stdcall* glBlendFunc)(u32 a1, u32 a2);

int _t_gl_error = 0;
#define gl_fail_if_false(x) x; if((_t_gl_error = glGetError()) != 0) { printf("gl_assert: %d\n", _t_gl_error); __debugbreak(); }

#pragma endregion

void opengl_init_pointers()
{
	HMODULE opengl_module_handle;

	fail_if_false(opengl_module_handle = GetModuleHandleA("opengl32.dll"));

	fail_if_false(glGetError = wgl_get_proc_address(opengl_module_handle, "glGetError"));
	fail_if_false(glBindBuffer = wgl_get_proc_address(opengl_module_handle, "glBindBuffer"));
	fail_if_false(glGenBuffers = wgl_get_proc_address(opengl_module_handle, "glGenBuffers"));
	fail_if_false(glBufferData = wgl_get_proc_address(opengl_module_handle, "glBufferData"));
	fail_if_false(glBufferSubData = wgl_get_proc_address(opengl_module_handle, "glBufferSubData"));
	fail_if_false(glGetShaderiv = wgl_get_proc_address(opengl_module_handle, "glGetShaderiv"));
	fail_if_false(glCreateShader = wgl_get_proc_address(opengl_module_handle, "glCreateShader"));
	fail_if_false(glShaderSource = wgl_get_proc_address(opengl_module_handle, "glShaderSource"));
	fail_if_false(glCompileShader = wgl_get_proc_address(opengl_module_handle, "glCompileShader"));
	fail_if_false(glCreateProgram = wgl_get_proc_address(opengl_module_handle, "glCreateProgram"));
	fail_if_false(glAttachShader = wgl_get_proc_address(opengl_module_handle, "glAttachShader"));
	fail_if_false(glLinkProgram = wgl_get_proc_address(opengl_module_handle, "glLinkProgram"));
	fail_if_false(glGetProgramiv = wgl_get_proc_address(opengl_module_handle, "glGetProgramiv"));
	fail_if_false(glGetProgramInfoLog = wgl_get_proc_address(opengl_module_handle, "glGetProgramInfoLog"));
	fail_if_false(glGetShaderInfoLog = wgl_get_proc_address(opengl_module_handle, "glGetShaderInfoLog"));
	fail_if_false(glDeleteShader = wgl_get_proc_address(opengl_module_handle, "glDeleteShader"));
	fail_if_false(glEnableVertexAttribArray = wgl_get_proc_address(opengl_module_handle, "glEnableVertexAttribArray"));
	fail_if_false(glVertexAttribPointer = wgl_get_proc_address(opengl_module_handle, "glVertexAttribPointer"));
	fail_if_false(glGenTextures = wgl_get_proc_address(opengl_module_handle, "glGenTextures"));
	fail_if_false(glBindTexture = wgl_get_proc_address(opengl_module_handle, "glBindTexture"));
	fail_if_false(glTexParameteri = wgl_get_proc_address(opengl_module_handle, "glTexParameteri"));
	fail_if_false(glTexImage2D = wgl_get_proc_address(opengl_module_handle, "glTexImage2D"));
	fail_if_false(glGenerateMipmap = wgl_get_proc_address(opengl_module_handle, "glGenerateMipmap"));
	fail_if_false(glDrawElements = wgl_get_proc_address(opengl_module_handle, "glDrawElements"));
	fail_if_false(glViewport = wgl_get_proc_address(opengl_module_handle, "glViewport"));
	fail_if_false(glClearColor = wgl_get_proc_address(opengl_module_handle, "glClearColor"));
	fail_if_false(glClear = wgl_get_proc_address(opengl_module_handle, "glClear"));
	fail_if_false(glUseProgram = wgl_get_proc_address(opengl_module_handle, "glUseProgram"));
	fail_if_false(glTranslatef = wgl_get_proc_address(opengl_module_handle, "glTranslatef"));
	fail_if_false(glRotatef = wgl_get_proc_address(opengl_module_handle, "glRotatef"));
	fail_if_false(glScalef = wgl_get_proc_address(opengl_module_handle, "glScalef"));
	fail_if_false(glMatrixMode = wgl_get_proc_address(opengl_module_handle, "glMatrixMode"));
	fail_if_false(glLoadIdentity = wgl_get_proc_address(opengl_module_handle, "glLoadIdentity"));
	fail_if_false(glRotatef = wgl_get_proc_address(opengl_module_handle, "glRotatef"));
	fail_if_false(glOrtho = wgl_get_proc_address(opengl_module_handle, "glOrtho"));
	fail_if_false(glBindBufferBase = wgl_get_proc_address(opengl_module_handle, "glBindBufferBase"));
	fail_if_false(glMapBuffer = wgl_get_proc_address(opengl_module_handle, "glMapBuffer"));
	fail_if_false(glUnmapBuffer = wgl_get_proc_address(opengl_module_handle, "glUnmapBuffer"));
	fail_if_false(glEnable = wgl_get_proc_address(opengl_module_handle, "glEnable"));
	fail_if_false(glDisable = wgl_get_proc_address(opengl_module_handle, "glDisable"));
	fail_if_false(glBlendFunc = wgl_get_proc_address(opengl_module_handle, "glBlendFunc"));
}

u32 opengl_compile_shader(const ch8* shader_source, const ch8* shader_type)
{
	u32 shader_type_id;

	if(shader_type == "vertex")
		shader_type_id = GL_VERTEX_SHADER;
	else if(shader_type == "fragment")
		shader_type_id = GL_FRAGMENT_SHADER;
	else
		fail_if_false(false);

	u32 shader = glCreateShader(shader_type_id);
	gl_fail_if_false(glShaderSource(shader, 1, &shader_source, NULL));
	gl_fail_if_false(glCompileShader(shader));

	int compilation_status;
	char compilation_log[512];
	gl_fail_if_false(glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status));
	if(!compilation_status)
	{
		gl_fail_if_false(glGetShaderInfoLog(shader, 512, NULL, compilation_log));
		trace_information(compilation_log);
		return 0;
	};
	return shader;
}

u32 opengl_create_shader_program(u32 vertex_shader, u32 fragment_shader)
{
	u32 program = glCreateProgram();

	gl_fail_if_false(glAttachShader(program, vertex_shader));
	gl_fail_if_false(glAttachShader(program, fragment_shader));
	gl_fail_if_false(glLinkProgram(program));

	i32 compilation_status;
	char compilation_log[512];
	gl_fail_if_false(glGetProgramiv(program, GL_LINK_STATUS, &compilation_status));
	if(!compilation_status)
	{
		gl_fail_if_false(glGetProgramInfoLog(program, 512, NULL, compilation_log));
		trace_information("%s", compilation_log);
	}
	// delete the shaders as they're linked into our program now and no longer necessary
	gl_fail_if_false(glDeleteShader(vertex_shader));
	gl_fail_if_false(glDeleteShader(fragment_shader));

	return program;
}

void openlgl_setup_vertex_attributes(u32 shader, const ShaderAttribute attributes[MAX_SHADER_ATTRIBUTES])
{
	u32 stride = 0, index = 0;
	intptr offset = 0;

	for(size_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++)
	{
		if(attributes[i].count == 0)
			break;

		if(attributes[i].type == GL_FLOAT)
			stride += 4 * attributes[i].count;
	}

	for(size_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++)
	{
		if(attributes[i].count == 0)
			break;

		u32 gl_type_id = 0;
		u32 type_size = 0;

		if(attributes[i].type == GL_FLOAT)
			gl_type_id = GL_FLOAT, type_size = 4;

		fail_if_false(gl_type_id && type_size);

		gl_fail_if_false(glEnableVertexAttribArray(index));
		gl_fail_if_false(glVertexAttribPointer(index, attributes[i].count, gl_type_id, false, stride, (const void*) offset));

		offset += attributes[i].count * type_size;
		index++;
	}
}


// Input

u8 g_input_key_states[0xFF];

//////////////////////////////////////////////////////////////////////
// Window

HWND g_window_handle;
HDC g_device_context;
HGLRC g_rendering_context;
u32 window_width = DEFAULT_WINDOW_WIDTH, window_height = DEFAULT_WINDOW_HEIGHT;
u32 g_cursor_x = 0, g_cursor_y = 0;

LRESULT CALLBACK window_proc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	switch(msg)
	{
		case WM_CHAR:
		{
			break;
		};
		case WM_KEYDOWN:
		{
			g_input_key_states[wparam] = 1;
			break;
		}
		case WM_KEYUP:
		{
			g_input_key_states[wparam] = 0;
			break;
		}
		case WM_SIZE:
		{
			RECT client_rect;
			fail_if_false(GetClientRect(window, &client_rect));
			window_width = (client_rect.right - client_rect.left);
			window_height = (client_rect.bottom - client_rect.top);
			if(glViewport)
			{
				glViewport(0, 0, window_width, window_height);
			}
			break;
		}
		case WM_MOUSEMOVE:
		{
			#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
			#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

			g_cursor_x = GET_X_LPARAM(lparam);
			g_cursor_y = GET_Y_LPARAM(lparam);

			break;
		}
		default:
			return DefWindowProcA(window, msg, wparam, lparam);
	}

	return result;
}

void window_init()
{
	i32 window_pixel_format;
	UINT num_window_pixel_formats;
	PIXELFORMATDESCRIPTOR pixel_format_desc;
	HMODULE module_handle;


	const char* window_class_name = "Hello";
	const char* window_title = "Hello";

	fail_if_false(module_handle = GetModuleHandle(NULL));

	WNDCLASSEXA window_class = {
		.cbSize = sizeof(WNDCLASSEXA),
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc = window_proc,
		.hInstance = module_handle,
		.hCursor = LoadCursor(0, IDC_ARROW),
		.lpszClassName = window_title
	};

	i32 pixel_format_attributes[] = {
		WGL_DRAW_TO_WINDOW_ARB, 1,
		WGL_SUPPORT_OPENGL_ARB, 1,
		WGL_DOUBLE_BUFFER_ARB, 1,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
	};

	i32 gl_attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0,
	};

	fail_if_false(RegisterClassExA(&window_class));

	fail_if_false(g_window_handle = CreateWindowExA(0, window_class_name, window_title,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, 0, module_handle, 0));

	fail_if_false(g_device_context = GetDC(g_window_handle));

	fail_if_false(wglChoosePixelFormatARB(g_device_context, pixel_format_attributes, 0, 1, &window_pixel_format, &num_window_pixel_formats));

	fail_if_false(DescribePixelFormat(g_device_context, window_pixel_format, sizeof(pixel_format_desc), &pixel_format_desc));

	fail_if_false(SetPixelFormat(g_device_context, window_pixel_format, &pixel_format_desc));

	fail_if_false(g_rendering_context = wglCreateContextAttribsARB(g_device_context, 0, gl_attributes));
	fail_if_false(wglMakeCurrent(g_device_context, g_rendering_context));

	fail_if_false(wglSwapIntervalEXT(false));

	ShowWindow(g_window_handle, SW_NORMAL);
}

void window_process_messages()
{
	MSG window_message;

	while(PeekMessage(&window_message, g_window_handle, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&window_message);
		DispatchMessage(&window_message);
	}
}

//////////////////////////////////////////////////////////////////////
// Sprite renderer

#pragma region

typedef struct {
	const char* name;
	u32 atlas_x, atlas_y;
	u32 sprite_width, sprite_height;
} AtlasSpriteInfo;

typedef struct {
	v2f screen_pos;
	s2f screen_size;
	v2f sprite_atlas_pos;
	s2f sprite_size;
	c4f color;

	float scale;
	bool full_screen;
	// TODO: Find out why this padding is needed
	float ff2;
	float ff3;

} AtlasSpriteRenderData;

Resource g_sprite_render_atlas_vertex_shader_resource;
Resource g_sprite_render_atlas_fragment_shader_resource;

#define g_sprite_renderer_texture_resources_max_count 64
Resource g_sprite_renderer_texture_resources[g_sprite_renderer_texture_resources_max_count];
u32 g_sprite_renderer_texture_resources_count = 0;

#define g_sprite_renderer_atlas_sprite_info_max_count g_sprite_renderer_texture_resources_max_count
AtlasSpriteInfo g_sprite_renderer_atlas_sprite_info[g_sprite_renderer_atlas_sprite_info_max_count];
u32 g_sprite_renderer_atlas_sprite_info_count = 0;

AtlasSpriteRenderData* g_sprite_renderer_render_buffer;
u32 g_sprite_renderer_render_buffer_count = 0;

u32 g_sprite_renderer_index_buffer_handle;
u32 g_sprite_renderer_storage_buffer_handle;
u32 g_sprite_renderer_atlas_shader_handle;
u32 g_sprite_renderer_atlas_texture_handle;

const ShaderAttribute g_sprite_render_shader_attributes[MAX_SHADER_ATTRIBUTES] = {
	{.type = GL_FLOAT, .count = 0},
};

#define VERTICES_PER_SPRITE 4
#define INDICES_PER_SPRITE 6

#define MAX_SPRITE_VERTICES 0x1000 - VERTICES_PER_SPRITE
#define MAX_SPRITE_INDICES 0x1000 - INDICES_PER_SPRITE

#define SIZE_OF_VERTEX 4
#define SIZE_OF_INDEX sizeof(u32)

#define SPRITE_VERTEX_BUFFER_SIZE MAX_SPRITE_VERTICES * SIZE_OF_VERTEX
#define SPRITE_INDEX_BUFFER_SIZE MAX_SPRITE_INDICES * SIZE_OF_INDEX

static_assert(VERTICES_PER_SPRITE% MAX_SPRITE_VERTICES == 0, "Unaligned vertex buffer");
static_assert(INDICES_PER_SPRITE% MAX_SPRITE_INDICES == 0, "Unaligned index  buffer");

#pragma endregion

const AtlasSpriteInfo* sprite_renderer_get_sprite_info_by_name(const ch8* sprite_name)
{
	for(size_t sprite_info_index = 0; sprite_info_index < g_sprite_renderer_atlas_sprite_info_count; sprite_info_index++)
	{
		AtlasSpriteInfo* sprite_info = &g_sprite_renderer_atlas_sprite_info[sprite_info_index];

		if(strstr(sprite_info->name, sprite_name))
		{
			return sprite_info;
		}
	}

	fail_if_false(false);

	return null;
}

AtlasSpriteRenderData* sprite_renderer_render_new_sprite(const AtlasSpriteInfo* sprite_info)
{
	AtlasSpriteRenderData* sprite = &g_sprite_renderer_render_buffer[g_sprite_renderer_render_buffer_count++];
	sprite->screen_size.w = (f32) sprite_info->sprite_width;
	sprite->screen_size.h = (f32) sprite_info->sprite_height;
	sprite->sprite_size.w = (f32) sprite_info->sprite_width;
	sprite->sprite_size.h = (f32) sprite_info->sprite_height;
	sprite->sprite_atlas_pos.x = (f32) sprite_info->atlas_x;
	sprite->sprite_atlas_pos.y = (f32) sprite_info->atlas_y;

	return sprite;
}

void sprite_renderer_use_atlas_shader()
{
	glUseProgram(g_sprite_renderer_atlas_shader_handle);
}

void sprite_renderer_init_atlas_storage_buffer()
{
	glGenBuffers(1, &g_sprite_renderer_storage_buffer_handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_sprite_renderer_storage_buffer_handle);
	glBufferData(GL_SHADER_STORAGE_BUFFER, g_sprite_renderer_atlas_sprite_info_max_count * sizeof(AtlasSpriteRenderData), 0, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_sprite_renderer_storage_buffer_handle);
}

void sprite_renderer_init_atlas_index_buffer()
{
	const u32 vertices_per_sprite = 4;
	const u32 indices_per_sprite = 6;
	const u32 size_of_index = 4;
	const u32 num_indices = g_sprite_renderer_atlas_sprite_info_max_count * indices_per_sprite;
	u32* sprite_indices;

	gl_fail_if_false(glGenBuffers(1, &g_sprite_renderer_index_buffer_handle));
	gl_fail_if_false(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_sprite_renderer_index_buffer_handle));
	gl_fail_if_false(glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * size_of_index, NULL, GL_STATIC_DRAW));

	gl_fail_if_false(sprite_indices = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for(u32 index_i = 0, vertex_i = 0;
		index_i < num_indices - indices_per_sprite;
		index_i += indices_per_sprite, vertex_i += vertices_per_sprite)
	{
		sprite_indices[index_i + 0] = vertex_i + 0;
		sprite_indices[index_i + 1] = vertex_i + 1;
		sprite_indices[index_i + 2] = vertex_i + 2;

		sprite_indices[index_i + 3] = vertex_i + 3;
		sprite_indices[index_i + 4] = vertex_i + 2;
		sprite_indices[index_i + 5] = vertex_i + 1;
	}

	gl_fail_if_false(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
}

bool sprite_renderer_init_atlas_shader()
{
	#ifdef OPENGL
	u32 vertex_shader = opengl_compile_shader(g_sprite_render_atlas_vertex_shader_resource.buffer, "vertex");
	u32 fragment_shader = opengl_compile_shader(g_sprite_render_atlas_fragment_shader_resource.buffer, "fragment");

	if(!vertex_shader || !fragment_shader)
		return false;

	g_sprite_renderer_atlas_shader_handle = opengl_create_shader_program(vertex_shader, fragment_shader);

	gl_fail_if_false(glUseProgram(g_sprite_renderer_atlas_shader_handle));

	return true;
	#endif

	fail_if_false(false);

	return true;
}

void sprite_renderer_init_atlas_texture()
{
	u32 atlas_width = 5000, atlas_height = 5000;
	u32 atlas_size = atlas_width * atlas_height;

	u8* atlas_data = (u8*) malloc(atlas_size * 4);
	fail_if_false(atlas_data);

	memset(atlas_data, 0, atlas_size * 4);

	g_sprite_renderer_atlas_sprite_info_count = 0;

	int x = 0, y = 0;
	for(u32 sprite_index = 0; sprite_index < g_sprite_renderer_texture_resources_count; sprite_index++)
	{
		i32 sprite_width, sprite_height, sprite_components;

		u8* sprite_data = stbi_load(g_sprite_renderer_texture_resources[sprite_index].path,
			&sprite_width, &sprite_height, &sprite_components, STBI_default);

		fail_if_false(sprite_data);

		g_sprite_renderer_atlas_sprite_info[sprite_index].name = g_sprite_renderer_texture_resources[sprite_index].path;
		g_sprite_renderer_atlas_sprite_info[sprite_index].atlas_x = x;
		g_sprite_renderer_atlas_sprite_info[sprite_index].atlas_y = y;
		g_sprite_renderer_atlas_sprite_info[sprite_index].sprite_width = sprite_width;
		g_sprite_renderer_atlas_sprite_info[sprite_index].sprite_height = sprite_height;
		g_sprite_renderer_atlas_sprite_info_count++;

		for(int row = 0; row < sprite_height; row++)
		{
			u32 atlas_row_offset = row * atlas_width;
			atlas_row_offset += x;

			u32 sprite_row_offset = row * sprite_width;

			memcpy(atlas_data + atlas_row_offset * 4,
				sprite_data + sprite_row_offset * 4,
				sprite_width * 4);
		}

		x += sprite_width;
		if(x + atlas_width > atlas_size) {
			x = 0;
			y += atlas_height;
		}

		fail_if_false(x < atlas_width&& y < atlas_height);

		stbi_image_free(sprite_data);
	}

	#ifdef OPENGL

	gl_fail_if_false(glGenTextures(1, &g_sprite_renderer_atlas_texture_handle));
	gl_fail_if_false(glBindTexture(GL_TEXTURE_2D, g_sprite_renderer_atlas_texture_handle));
	// set the texture wrapping/filtering options (on the currently bound texture object)
	gl_fail_if_false(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	gl_fail_if_false(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	gl_fail_if_false(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	gl_fail_if_false(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// generate the texture (and mipmap)
	gl_fail_if_false(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_width, atlas_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_data));
	gl_fail_if_false(glGenerateMipmap(GL_TEXTURE_2D));

	#endif

	free(atlas_data);
}

void sprite_renderer_push_sprite(const ch8* sprite_name, i32 x, i32 y, f32 scale, bool full_screen, const c4f* color)
{
	const AtlasSpriteInfo* sprite_info = sprite_renderer_get_sprite_info_by_name(sprite_name);
	AtlasSpriteRenderData* sprite = sprite_renderer_render_new_sprite(sprite_info);

	sprite->screen_pos.x = (f32) x;
	sprite->screen_pos.y = (f32) y;
	sprite->scale = scale;
	sprite->full_screen = full_screen;
	sprite->color = *color;
}

void sprite_renderer_render_begin()
{
	gl_fail_if_false(glClearColor(1, 0, 0, 1));
	gl_fail_if_false(glClear(GL_COLOR_BUFFER_BIT));

	gl_fail_if_false(g_sprite_renderer_render_buffer = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));

	g_sprite_renderer_render_buffer_count = 0;
}

void sprite_renderer_render_end()
{
	gl_fail_if_false(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));
	gl_fail_if_false(glDrawElements(GL_TRIANGLES, g_sprite_renderer_render_buffer_count * 6, GL_UNSIGNED_INT, 0));

	fail_if_false(SwapBuffers(g_device_context));
}

void sprite_renderer_push_text(const char* text, float scale)
{
	u32 text_length = str_length(text);

	u32 x_offset = 0, y_offset = 0;

	for(size_t character_index = 0; character_index < text_length; character_index++)
	{
		ch8 texture_name[MAX_PATH];
		sprintf_s(texture_name, sizeof(texture_name), "c_%c.png", text[character_index]);

		for(size_t sprite_index = 0; sprite_index < g_sprite_renderer_atlas_sprite_info_count; sprite_index++)
		{
			AtlasSpriteInfo* sprite_info = &g_sprite_renderer_atlas_sprite_info[sprite_index];

			if(strstr(sprite_info->name, texture_name))
			{
				sprite_renderer_push_sprite(texture_name, x_offset, 0, scale / 3, false, &C4F_WHITE);

				x_offset += sprite_info->sprite_width + 1;
			}
		}
	}

}

void sprite_renderer_init()
{
	sprite_renderer_init_atlas_index_buffer();

	sprite_renderer_init_atlas_storage_buffer();

	sprite_renderer_init_atlas_shader();

	sprite_renderer_use_atlas_shader();

	sprite_renderer_init_atlas_texture();
}

//////////////////////////////////////////////////////////////////////
// Utils

inline u64 get_tick_count()
{
	LARGE_INTEGER li;

	fail_if_false(QueryPerformanceCounter(&li));

	return li.QuadPart;
}

inline u64 get_ticks_per_second()
{
	LARGE_INTEGER li;

	fail_if_false(QueryPerformanceFrequency(&li));

	return li.QuadPart;
}

//////////////////////////////////////////////////////////////////////
// Main

void init_resources()
{
	resource_init_from_path("./assets/sprite.vert", &g_sprite_render_atlas_vertex_shader_resource);
	resource_init_from_path("./assets/sprite.frag", &g_sprite_render_atlas_fragment_shader_resource);

	resource_init_from_folder("./assets/textures", g_sprite_renderer_texture_resources, &g_sprite_renderer_texture_resources_count, g_sprite_renderer_texture_resources_max_count);
}

void hot_reload()
{
	if(resource_update(&g_sprite_render_atlas_vertex_shader_resource))
		sprite_renderer_init_atlas_shader();

	if(resource_update(&g_sprite_render_atlas_fragment_shader_resource))
		sprite_renderer_init_atlas_shader();

	bool update_sprite_atlas = false;

	for(size_t i = 0; i < g_sprite_renderer_texture_resources_count; i++)
	{
		if(resource_update(&g_sprite_renderer_texture_resources[i]))
		{
			update_sprite_atlas = true;
			break;
		}
	}

	if(update_sprite_atlas)
	{
		sprite_renderer_init_atlas_texture();
	}
}

//////////////////////////////////////////////////////////////////////
// Game

typedef struct {
	v2f position;
} GameObject;

GameObject game_objects[10];
u32 game_objects_count = 0;

void game_init()
{
	GameObject* game_object = &game_objects[game_objects_count++];
}

void game_render()
{
	sprite_renderer_render_begin();

	for(size_t i = 0; i < game_objects_count; i++)
	{
		GameObject* game_object = &game_objects[i];

		sprite_renderer_push_sprite("radio.png", DEFAULT_WINDOW_WIDTH - 48, 0, 1.0, false, &C4F_WHITE);


		sprite_renderer_push_text("hello", 15.0f);

	}
	sprite_renderer_render_end();
}

//////////////////////////////////////////////////////////////////////
// Main

int main(int argc, const char** argv)
{
	init_resources();

	wgl_init_opengl();

	window_init();

	opengl_init_pointers();

	sprite_renderer_init();

	game_init();

	while(true)
	{
		#ifdef _DEBUG
		hot_reload();
		#endif

		window_process_messages();

		game_render();

		Sleep(1);
	}

	return 0;
}