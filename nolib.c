#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma comment(lib, "opengl32.lib")

#define TRACE(fmt, ...) printf(fmt, __VA_ARGS__);

#ifdef _DEBUG
#define dcheck(x) assert(x)
#else
#define dcheck(x) x
#endif

typedef float f32;
typedef unsigned char u8;
typedef unsigned long u32;
typedef long i32;
typedef long long i64;
typedef unsigned long long u64;

#define intptr intptr_t
#define uintptr uintptr_t

#define nullptr 0
#define nil void

#define TYPE_ID_NONE 0
#define TYPE_ID_FLOAT 1
#define TYPE_SIZE_FLOAT sizeof(f32)

typedef struct {
	float x, y;
} v2f;

typedef struct {
	float x, y, z;
} v3f;

typedef struct {
	float r, g, b;
} c3f;

typedef struct {
	float r, g, b, a;
} c4f;

typedef struct {
	float x, y, z, w;
} v4f;

typedef struct {
	float w, h;
} s2f;

#define true 1
#define false 0

#define MAX_ENTITIES 1000
#define HAS_TEXTURE (1 << 0)
#define HAS_POSITION (1 << 1)

#define VERTICES_PER_SPRITE 4
#define INDICES_PER_SPRITE 6

#define MAX_SPRITE_VERTICES 0x1000 - VERTICES_PER_SPRITE
#define MAX_SPRITE_INDICES 0x1000 - INDICES_PER_SPRITE

#define SIZE_OF_VERTEX 4
#define SIZE_OF_INDEX 4

#define SPRITE_VERTEX_BUFFER_SIZE MAX_SPRITE_VERTICES * SIZE_OF_VERTEX
#define SPRITE_INDEX_BUFFER_SIZE MAX_SPRITE_INDICES * SIZE_OF_INDEX

#define MAX_SHADER_ATTRIBUTES 15

#define WINDOW_TITLE "hehe"
#define WINDOW_CLASS_NAME "hehe"
#define WINDOW_DEFAULT_WIDTH 800
#define WINDOW_DEFAULT_HEIGHT 800

#define DUMMY_WINDOW_TITLE "_dummy"
#define DUMMY_WINDOW_CLASS_NAME "_dummy"

static_assert(VERTICES_PER_SPRITE% MAX_SPRITE_VERTICES == 0, "Unaligned vertex buffer");
static_assert(INDICES_PER_SPRITE% MAX_SPRITE_INDICES == 0, "Unaligned index  buffer");

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

HGLRC(__stdcall* wglCreateContextAttribsARB)(HDC hdc, HGLRC hShareContext, const int* attribList);
BOOL(__stdcall* wglChoosePixelFormatARB)(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
void* (__stdcall* wglGetProcAddress2)(LPCSTR name);
void* (__stdcall* wglSwapIntervalEXT)(u8 vsync);

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
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

nil(__stdcall* glBindBuffer)(u32 target, u32 buffer);
nil(__stdcall* glGenBuffers)(i32 count, u32* buffers);
nil(__stdcall* glBufferData)(u32 target, size_t size, const void* data, u32 usage);
nil(__stdcall* glBufferSubData)(u32 target, intptr offset, uintptr size, const void* data);
nil(__stdcall* glGetShaderiv)(u32 program, u32 pname, int* params);
u32(__stdcall* glCreateShader)(u32 type);
nil(__stdcall* glShaderSource)(u32 _shader, i32 count, const char* const* string, const int* length);
nil(__stdcall* glCompileShader)(u32 _shader);
u32(__stdcall* glCreateProgram)(void);
nil(__stdcall* glAttachShader)(u32 program, u32 _shader);
nil(__stdcall* glLinkProgram)(u32 program);
nil(__stdcall* glGetProgramiv)(u32 program, u32 pname, int* params);
nil(__stdcall* glGetProgramInfoLog)(u32 program, i32 bufSize, i32* length, char* infoLog);
nil(__stdcall* glGetShaderInfoLog)(u32 _shader, i32 bufSize, i32* length, char* infoLog);
nil(__stdcall* glDeleteShader)(u32 _shader);
nil(__stdcall* glEnableVertexAttribArray)(u32 index);
nil(__stdcall* glVertexAttribPointer)(u32 index, i32 size, u32 type, u8 normalized, i32 stride, const void* pointer);
nil(__stdcall* glGenTextures)(i32 n, u32* textures);
nil(__stdcall* glBindTexture)(u32 target, u32 _atlas_texture);
nil(__stdcall* glTexParameteri)(u32 target, u32 pname, u32 params);
nil(__stdcall* glTexImage2D)(u32 target, i32 level, i32 internalformat, i32 width, i32 height, i32 border, u32 format, u32 type, const void* pixels);
nil(__stdcall* glGenerateMipmap)(u32 target);
nil(__stdcall* glDrawElements)(u32 mode, i32 count, u32 type, const void* indices);
nil(__stdcall* glViewport)(i32 x, i32 y, i32 width, i32 height);
nil(__stdcall* glClearColor)(f32 red, f32 green, f32 blue, f32 alpha);
nil(__stdcall* glClear)(u32 mask);
nil(__stdcall* glUseProgram)(u32 program);
u32(__stdcall* glGetError)();

int _t_gl_error = 0;
#define gl_dcheck(x) x; if((_t_gl_error = glGetError()) != 0) { printf("gl_assert: %d\n", _t_gl_error); __debugbreak(); }

typedef struct {
	u32 type;
	u32 count;
} ShaderAttribute;

const v2f TOP_LEFT_NDC_V2F = {-1.f, -1.f};
const v2f TOP_RIGHT_NDC_V2F = {-1.f, +1.f};
const v2f BOT_LEFT_NDC_V2F = {+1.f, -1.f};
const v2f BOT_RIGHT_NDC_V2F = {+1.f, +1.f};

const v2f TOP_LEFT_TEX_V2F = {1.f, 1.f};
const v2f TOP_RIGHT_TEX_V2F = {0.f, 1.f};
const v2f BOT_LEFT_TEX_V2F = {1.f, 0.f};
const v2f BOT_RIGHT_TEX_V2F = {0.f, 0.f};

const s2f TEX_ATLAS_SIZE = {512.f, 512.f};

const c3f WHITE = {1.f, 1.f, 1.f};

HMODULE opengl_module;

void* gl_get_proc_address(const char* name)
{
	void* result = 0;

	result = wglGetProcAddress(name);

	if(result) return result;

	return GetProcAddress(opengl_module, name);
}

void begin_gl_setup()
{

}

void end_gl_setup()
{

}

u32 compile_shader(const char* shader_source, const char* shader_type)
{
	i32 shader_type_id;

	if(shader_type == "vertex")
		shader_type_id = GL_VERTEX_SHADER;
	else if(shader_type == "fragment")
		shader_type_id = GL_FRAGMENT_SHADER;
	else
		dcheck(false);

	u32 shader = glCreateShader(shader_type_id);
	gl_dcheck(glShaderSource(shader, 1, &shader_source, NULL));
	gl_dcheck(glCompileShader(shader));

	int compilation_status;
	char compilation_log[512];
	gl_dcheck(glGetShaderiv(shader, GL_COMPILE_STATUS, &compilation_status));
	if(!compilation_status)
	{
		gl_dcheck(glGetShaderInfoLog(shader, 512, NULL, compilation_log));
		dcheck(false);
	};
	return shader;
}

void setup_vertex_attributes(u32 shader, const ShaderAttribute attributes[MAX_SHADER_ATTRIBUTES])
{
	u32 stride = 0, index = 0;
	u8* offset = 0;

	for(size_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++)
	{
		if(attributes[i].count == 0)
			break;

		if(attributes[i].type == TYPE_ID_FLOAT)
			stride += TYPE_SIZE_FLOAT * attributes[i].count;
	}

	for(size_t i = 0; i < MAX_SHADER_ATTRIBUTES; i++)
	{
		if(attributes[i].count == 0)
			break;

		u32 gl_type_id = 0;
		u32 type_size = 0;

		if(attributes[i].type == TYPE_ID_FLOAT)
			gl_type_id = GL_FLOAT, type_size = TYPE_SIZE_FLOAT;

		dcheck(gl_type_id && type_size);

		gl_dcheck(glEnableVertexAttribArray(index));
		gl_dcheck(glVertexAttribPointer(index, attributes[i].count, gl_type_id, false, stride, offset));

		offset += attributes[i].count * type_size;
		index++;
	}
}

u8 use_shader(u32 shader)
{
	gl_dcheck(glUseProgram(shader));

	return true;
}


u32 create_shader(const char* vertex_shader_source, const char* fragment_shader_source, const ShaderAttribute attributes[MAX_SHADER_ATTRIBUTES])
{
	u32 fragment_shader = compile_shader(vertex_shader_source, "vertex");
	u32 vertex_shader = compile_shader(fragment_shader_source, "fragment");
	u32 program = glCreateProgram();

	gl_dcheck(glAttachShader(program, vertex_shader));
	gl_dcheck(glAttachShader(program, fragment_shader));
	gl_dcheck(glLinkProgram(program));

	i32 compilation_status;
	char compilation_log[512];
	gl_dcheck(glGetProgramiv(program, GL_LINK_STATUS, &compilation_status));
	if(!compilation_status)
	{
		gl_dcheck(glGetProgramInfoLog(program, 512, NULL, compilation_log));
		dcheck(false);
	}
	// delete the shaders as they're linked into our program now and no longer necessary
	gl_dcheck(glDeleteShader(vertex_shader));
	gl_dcheck(glDeleteShader(fragment_shader));

	setup_vertex_attributes(program, attributes);

	return program;
}

#define TEXTURE_WALL 1


u8 load_texture(u32 texture_id, const void** data, u32* width, u32* height, u32* num_channels)
{
	const char* path = 0;

	if(texture_id == TEXTURE_WALL)
		path = "./wall.jpg";

	dcheck(path);

	const void* result = stbi_load(path, width, height, num_channels, 0);

	if(!result)
		return false;

	*data = result;

	return true;
}
u32 create_texture_from_memory(const void* data, u32 width, u32 height, u32 num_channels)
{
	u32 texture;
	gl_dcheck(glGenTextures(1, &texture));
	gl_dcheck(glBindTexture(GL_TEXTURE_2D, texture));
	// set the texture wrapping/filtering options (on the currently bound texture object)
	gl_dcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	gl_dcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	gl_dcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	gl_dcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// generate the texture (and mipmap)
	gl_dcheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
	gl_dcheck(glGenerateMipmap(GL_TEXTURE_2D));
	return texture;
}

u32 create_texture(u32 texture_id)
{
	u32 texture;

	void* data;
	u32 width, height, num_channels;
	dcheck(load_texture(texture_id, &data, &width, &height, &num_channels));

	texture = create_texture_from_memory(data, width, height, num_channels);

	stbi_image_free(data);

	return texture;
}

u32 create_vertex_buffer(size_t size, u8 readonly, const void* initial_data)
{
	u32 vertex_buffer = 0;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, size, initial_data, readonly ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	return vertex_buffer;
}

u32 create_index_buffer(size_t size, u8 readonly, const void* initial_data)
{
	u32 index_buffer = 0;
	gl_dcheck(glGenBuffers(1, &index_buffer));
	gl_dcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer));
	gl_dcheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, initial_data, readonly ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
	return index_buffer;
}

u8 bind_vertex_buffer(u32 vbo)
{
	gl_dcheck(glBindBuffer(GL_ARRAY_BUFFER, vbo));

	return true;
}

u8 bind_index_buffer(u32 ibo)
{
	gl_dcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

	return true;
}

u8 bind_texture(u32 texture_handle)
{
	gl_dcheck(glBindTexture(GL_TEXTURE_2D, texture_handle));

	return true;
}

void write_index_buffer(u32 buffer_handle, const void* data, uintptr data_size)
{
	gl_dcheck(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, data_size, data));
}

void write_vertex_buffer(u32 vbo, const void* data, uintptr data_size)
{
	gl_dcheck(glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, data));
}

void draw_triangles(i32 index_count)
{
	gl_dcheck(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0));
}

void clear_background()
{
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

const char* sprite_vertex_shader = "								\n\
	#version 330 core												\n\
	layout (location = 0) in vec2 a_pos;							\n\
	layout (location = 1) in vec4 a_color;							\n\
	layout (location = 2) in vec2 a_texture_coords;					\n\
	layout (location = 3) in vec2 a_texture_size;					\n\
	out vec4 a_color_;												\n\
	out vec2 a_texture_coords_;										\n\
	void main()														\n\
	{																\n\
		a_color_ = a_color;											\n\
		a_texture_coords_ = a_texture_coords;						\n\
																	\n\
		gl_Position = vec4(a_pos, 0.0, 1.0);						\n\
	};";

const char* sprite_fragment_shader = "								\n\
	#version 330 core												\n\
	in vec4 a_color_;												\n\
	in vec2 a_texture_coords_;										\n\
	out vec4 color;													\n\
	uniform sampler2D image;										\n\
	void main()														\n\
	{																\n\
		color = vec4(a_color_) * texture(image, a_texture_coords_);	\n\
	}";


const ShaderAttribute sprite_shader_attributes[MAX_SHADER_ATTRIBUTES] = {
	{.type = TYPE_ID_FLOAT, .count = 2},
	{.type = TYPE_ID_FLOAT, .count = 3},
	{.type = TYPE_ID_FLOAT, .count = 2},
	{.type = TYPE_ID_FLOAT, .count = 2},
};

typedef struct {
	v2f position;
	c3f color;
	v2f texture_offset;
	s2f texture_size;
} SpriteVertex;

SpriteVertex sprite_vertices[MAX_SPRITE_VERTICES];
i64 num_sprite_vertices = 0;

u32 sprite_indices[MAX_SPRITE_INDICES];
i32 num_sprite_indices = 0;

u32 window_width = WINDOW_DEFAULT_WIDTH, window_height = WINDOW_DEFAULT_HEIGHT;
u32 world_width = WINDOW_DEFAULT_WIDTH, world_height = WINDOW_DEFAULT_HEIGHT;

u32 entity_count = 0;
u64 entity_flags[MAX_ENTITIES];
v3f entity_positions[MAX_ENTITIES];

f32 vertices[MAX_SPRITE_VERTICES];
u32 num_vertices;

u32 indices[MAX_SPRITE_VERTICES];
u32 max_vertices;

#define SIMULATIONS_PER_SECOND 64
#define SIMULATION_TIME_INTERVAL (1.f / (f32)SIMULATIONS_PER_SECOND)
#define RENDER_TIME_INTERVAL SIMULATION_TIME_INTERVAL

inline u8 screen_to_ndc_v2f(const v2f* screen_pos, v2f* ndc_pos)
{
	ndc_pos->x = ((screen_pos->x / window_width) * 2) - 1.f;
	ndc_pos->y = ((screen_pos->y / window_height) * 2) - 1.f;

	return true;
}

inline u8 world_to_ndc_v2f(const v3f* world_pos, v2f* ndc_pos)
{
	ndc_pos->x = ((world_pos->x / world_width) * 2) - 1.f;
	ndc_pos->y = ((world_pos->y / world_height) * 2) - 1.f;

	return true;
}

inline u64 get_tick_count()
{
	LARGE_INTEGER li;

	dcheck(QueryPerformanceCounter(&li));

	return li.QuadPart;
}

inline u64 get_ticks_per_second()
{
	LARGE_INTEGER li;

	dcheck(QueryPerformanceFrequency(&li));

	return li.QuadPart;
}

LRESULT CALLBACK window_proc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	switch(msg)
	{
		case WM_SIZE:
		{
			RECT client_rect;
			dcheck(GetClientRect(window, &client_rect));
			window_width = (client_rect.right - client_rect.left);
			window_height = (client_rect.bottom - client_rect.top);
			break;
		}

		default:
			return DefWindowProcA(window, msg, wparam, lparam);
	}

	return result;
}

int main(int argc, const char** argv)
{
	HWND dummy_window_handle;
	HDC dummy_device_context;
	int dummy_pixel_format;
	HGLRC dummy_rendering_context;

	HWND window_handle;
	HDC device_context;
	HGLRC rendering_context;
	i32 window_pixel_format;
	UINT num_window_pixel_formats;
	PIXELFORMATDESCRIPTOR pixel_format_desc;
	u32 sprite_index_buffer;
	u32 sprite_vertex_buffer;
	u32 sprite_shader;
	u32 test_texture;
	MSG window_message;
	u64 tick_count;
	u64 start_tick_count;
	u64 ticks_per_second;
	f32 time;
	f32 time_since_simulation;
	f32 last_simulation_time = -1.f;
	f32 time_since_render;
	f32 last_render_time = -1.f;
	u64 simulation_count = 0;
	u64 frame_count = 0;
	f32 last_printf_time = 0.f;

	HMODULE module_handle;

	dcheck(module_handle = GetModuleHandle(nullptr));
	dcheck(opengl_module = LoadLibraryA("opengl32.dll"));

	WNDCLASSEXA dummy_window_class = {
		.cbSize = sizeof(WNDCLASSEXA),
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc = DefWindowProcA,
		.hInstance = module_handle,
		.hCursor = LoadCursor(0, IDC_ARROW),
		.lpszClassName = DUMMY_WINDOW_CLASS_NAME
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

	dcheck(RegisterClassExA(&dummy_window_class));

	dcheck(dummy_window_handle = CreateWindowExA(0, DUMMY_WINDOW_CLASS_NAME, DUMMY_WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, 0, 0, module_handle, 0));

	dcheck(dummy_device_context = GetDC(dummy_window_handle));

	dcheck(dummy_pixel_format = ChoosePixelFormat(dummy_device_context, &dummy_pixel_format_desc));

	dcheck(SetPixelFormat(dummy_device_context, dummy_pixel_format, &dummy_pixel_format_desc));

	dcheck(dummy_rendering_context = wglCreateContext(dummy_device_context));

	dcheck(wglMakeCurrent(dummy_device_context, dummy_rendering_context));

	dcheck(wglChoosePixelFormatARB = gl_get_proc_address("wglChoosePixelFormatARB"));
	dcheck(wglCreateContextAttribsARB = gl_get_proc_address("wglCreateContextAttribsARB"));
	dcheck(wglSwapIntervalEXT = gl_get_proc_address("wglSwapIntervalEXT"));

	dcheck(glGetError = gl_get_proc_address("glGetError"));
	dcheck(glBindBuffer = gl_get_proc_address("glBindBuffer"));
	dcheck(glGenBuffers = gl_get_proc_address("glGenBuffers"));
	dcheck(glBufferData = gl_get_proc_address("glBufferData"));
	dcheck(glBufferSubData = gl_get_proc_address("glBufferSubData"));
	dcheck(glGetShaderiv = gl_get_proc_address("glGetShaderiv"));
	dcheck(glCreateShader = gl_get_proc_address("glCreateShader"));
	dcheck(glShaderSource = gl_get_proc_address("glShaderSource"));
	dcheck(glCompileShader = gl_get_proc_address("glCompileShader"));
	dcheck(glCreateProgram = gl_get_proc_address("glCreateProgram"));
	dcheck(glAttachShader = gl_get_proc_address("glAttachShader"));
	dcheck(glLinkProgram = gl_get_proc_address("glLinkProgram"));
	dcheck(glGetProgramiv = gl_get_proc_address("glGetProgramiv"));
	dcheck(glGetProgramInfoLog = gl_get_proc_address("glGetProgramInfoLog"));
	dcheck(glGetShaderInfoLog = gl_get_proc_address("glGetShaderInfoLog"));
	dcheck(glDeleteShader = gl_get_proc_address("glDeleteShader"));
	dcheck(glEnableVertexAttribArray = gl_get_proc_address("glEnableVertexAttribArray"));
	dcheck(glVertexAttribPointer = gl_get_proc_address("glVertexAttribPointer"));
	dcheck(glGenTextures = gl_get_proc_address("glGenTextures"));
	dcheck(glBindTexture = gl_get_proc_address("glBindTexture"));
	dcheck(glTexParameteri = gl_get_proc_address("glTexParameteri"));
	dcheck(glTexImage2D = gl_get_proc_address("glTexImage2D"));
	dcheck(glGenerateMipmap = gl_get_proc_address("glGenerateMipmap"));
	dcheck(glDrawElements = gl_get_proc_address("glDrawElements"));
	dcheck(glViewport = gl_get_proc_address("glViewport"));
	dcheck(glClearColor = gl_get_proc_address("glClearColor"));
	dcheck(glClear = gl_get_proc_address("glClear"));
	dcheck(glUseProgram = gl_get_proc_address("glUseProgram"));

	dcheck(wglMakeCurrent(dummy_device_context, 0));

	dcheck(wglDeleteContext(dummy_rendering_context));

	dcheck(ReleaseDC(dummy_window_handle, dummy_device_context));

	dcheck(DestroyWindow(dummy_window_handle));

	WNDCLASSEXA window_class = {
		.cbSize = sizeof(WNDCLASSEXA),
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.lpfnWndProc = window_proc,
		.hInstance = module_handle,
		.hCursor = LoadCursor(0, IDC_ARROW),
		.lpszClassName = WINDOW_TITLE
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

	dcheck(RegisterClassExA(&window_class));

	dcheck(window_handle = CreateWindowExA(0, WINDOW_CLASS_NAME, WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, 0, 0, module_handle, 0));

	dcheck(device_context = GetDC(window_handle));

	dcheck(wglChoosePixelFormatARB(device_context, pixel_format_attributes, 0, 1, &window_pixel_format, &num_window_pixel_formats));

	dcheck(DescribePixelFormat(device_context, window_pixel_format, sizeof(pixel_format_desc), &pixel_format_desc));

	dcheck(SetPixelFormat(device_context, window_pixel_format, &pixel_format_desc));

	dcheck(rendering_context = wglCreateContextAttribsARB(device_context, 0, gl_attributes));
	dcheck(wglMakeCurrent(device_context, rendering_context));

	wglSwapIntervalEXT(false);

	ShowWindow(window_handle, SW_SHOW);

	for(u32 i = 0, j = 0;
		i < MAX_SPRITE_INDICES - INDICES_PER_SPRITE;
		i += INDICES_PER_SPRITE, j += VERTICES_PER_SPRITE)
	{
		sprite_indices[i + 0] = j + 0;
		sprite_indices[i + 1] = j + 1;
		sprite_indices[i + 2] = j + 3;
		sprite_indices[i + 3] = j + 2;
		sprite_indices[i + 4] = j + 3;
		sprite_indices[i + 5] = j + 0;
	}

	begin_gl_setup();

	dcheck(sprite_index_buffer = create_index_buffer(SPRITE_INDEX_BUFFER_SIZE, true, sprite_indices));
	dcheck(sprite_vertex_buffer = create_vertex_buffer(SPRITE_VERTEX_BUFFER_SIZE, false, nullptr));
	dcheck(sprite_shader = create_shader(sprite_vertex_shader, sprite_fragment_shader, sprite_shader_attributes));
	dcheck(use_shader(sprite_shader));
	dcheck(test_texture = create_texture(TEXTURE_WALL));
	dcheck(bind_texture(test_texture));

	end_gl_setup();

	entity_flags[entity_count] |= HAS_POSITION | HAS_TEXTURE;
	entity_count++;

	dcheck(ticks_per_second = get_ticks_per_second());
	dcheck(start_tick_count = get_tick_count());

	while(true)
	{
		while(PeekMessage(&window_message, window_handle, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&window_message);
			DispatchMessage(&window_message);
		}

		dcheck(tick_count = get_tick_count());

		time = (f32) (tick_count - start_tick_count) / (f32) ticks_per_second;

		time_since_simulation = time - last_simulation_time;

		if(time_since_simulation >= SIMULATION_TIME_INTERVAL)
		{
			for(u64 entity = 0; entity < entity_count; entity++)
			{
				if(entity_flags[entity] & HAS_POSITION)
				{
					entity_positions[entity].x += 1.f;
					entity_positions[entity].y += 1.f;
					//entity_positions[entity].z += 1.;
				}
			}

			simulation_count++;
			last_simulation_time = time;
		}

		time_since_render = time - last_render_time;

		if(time_since_render >= RENDER_TIME_INTERVAL)
		{
			{
				SpriteVertex* sprite_vertex = &sprite_vertices[num_sprite_vertices];
				sprite_vertex->position = TOP_LEFT_NDC_V2F;
				sprite_vertex->texture_offset = TOP_LEFT_TEX_V2F;
				sprite_vertex->texture_size = TEX_ATLAS_SIZE;
				sprite_vertex->color = WHITE;

				sprite_vertex++;

				sprite_vertex->position = TOP_RIGHT_NDC_V2F;
				sprite_vertex->texture_offset = TOP_RIGHT_TEX_V2F;
				sprite_vertex->texture_size = TEX_ATLAS_SIZE;
				sprite_vertex->color = WHITE;

				sprite_vertex++;

				sprite_vertex->position = BOT_LEFT_NDC_V2F;
				sprite_vertex->texture_offset = BOT_LEFT_TEX_V2F;
				sprite_vertex->texture_size = TEX_ATLAS_SIZE;
				sprite_vertex->color = WHITE;

				sprite_vertex++;

				sprite_vertex->position = BOT_RIGHT_NDC_V2F;
				sprite_vertex->texture_offset = BOT_RIGHT_TEX_V2F;
				sprite_vertex->texture_size = TEX_ATLAS_SIZE;
				sprite_vertex->color = WHITE;

				num_sprite_vertices += VERTICES_PER_SPRITE;
				num_sprite_indices += INDICES_PER_SPRITE;
			}

			for(u64 entity = 0; entity < entity_count; entity++)
			{
				if(entity_flags[entity] & HAS_TEXTURE && entity_flags[entity] & HAS_POSITION)
				{
					SpriteVertex* sprite_vertex = &sprite_vertices[num_sprite_vertices];
					dcheck(world_to_ndc_v2f(&entity_positions[entity], &sprite_vertex->position));
					sprite_vertex->texture_offset = TOP_LEFT_TEX_V2F;
					sprite_vertex->texture_size = TEX_ATLAS_SIZE;
					sprite_vertex->color = WHITE;

					sprite_vertex++;

					sprite_vertex->position = TOP_RIGHT_NDC_V2F;
					sprite_vertex->texture_offset = TOP_RIGHT_TEX_V2F;
					sprite_vertex->texture_size = TEX_ATLAS_SIZE;
					sprite_vertex->color = WHITE;

					sprite_vertex++;

					sprite_vertex->position = BOT_LEFT_NDC_V2F;
					sprite_vertex->texture_offset = BOT_LEFT_TEX_V2F;
					sprite_vertex->texture_size = TEX_ATLAS_SIZE;
					sprite_vertex->color = WHITE;

					sprite_vertex++;

					sprite_vertex->position = BOT_RIGHT_NDC_V2F;
					sprite_vertex->texture_offset = BOT_RIGHT_TEX_V2F;
					sprite_vertex->texture_size = TEX_ATLAS_SIZE;
					sprite_vertex->color = WHITE;

					num_sprite_vertices += VERTICES_PER_SPRITE;
					num_sprite_indices += INDICES_PER_SPRITE;
				}
			}

			clear_background();

			write_vertex_buffer(sprite_vertex_buffer, sprite_vertices, num_sprite_vertices * sizeof(SpriteVertex));

			draw_triangles(num_sprite_indices);

			SwapBuffers(device_context);

			frame_count++;
			last_render_time = time;
		}

		if(time - last_printf_time > 1.f)
		{
			const float avg_fps = (f32) frame_count / time;
			const float avg_simulations = (f32) simulation_count / time;

			TRACE("Simulation count %I64d Frame count %I64d Time %0.2f Avg FPS %0.2f, Avg simulations %0.2f\n", simulation_count, frame_count, time, avg_fps, avg_simulations);

			last_printf_time = time;
		}

		num_sprite_vertices = 0, num_sprite_indices = 0;
	}
}