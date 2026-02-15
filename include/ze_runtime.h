#ifndef __ZE_RUNTIME_H__
#define __ZE_RUNTIME_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ZE_WIDTH 480
#define ZE_HEIGHT 360

#if defined(_ZADRAPANIE)
#include <cJSON.h>
#include <miniaudio.h>
#include <stb_ds.h>
#include <stb_image.h>
#include <nanosvg.h>
#include <nanosvgrast.h>

#include <cairo/cairo.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#if defined(_ZADRAPANIE) && defined(_WIN32)
#define ZEDECL extern __declspec(dllexport)
#elif defined(_WIN32)
#define ZEDECL extern __declspec(dllimport)
#else
#define ZEDECL extern
#endif

typedef unsigned char ze_bool;

#define ze_false ((ze_bool)0)
#define ze_true ((ze_bool)1)

typedef struct ze_runtime ze_runtime_t;
typedef struct ze_target  ze_target_t;
typedef struct ze_costume ze_costume_t;
typedef struct ze_block	  ze_block_t;
typedef struct ze_blockkv ze_blockkv_t;
typedef struct ze_thread  ze_thread_t;
typedef union ze_input	  ze_input_t;
typedef struct ze_inputkv ze_inputkv_t;
typedef struct ze_sprite  ze_sprite_t;

typedef unsigned char* (*ze_load_file_t)(ze_runtime_t* rt, const char* path, int* size);
typedef ze_bool (*ze_check_loop_t)(ze_thread_t* thread);

enum ze_input_type {
	ze_input_number = 0, /* treat 4/5/6/7/8 as same thing */
	ze_input_color,
	ze_input_string,
	ze_input_broadcast,
	ze_input_variable,
	ze_input_list
};

#if defined(_ZADRAPANIE)
typedef struct ze_texture ze_texture_t;

typedef cJSON	   ze_cJSON;
typedef GLFWwindow ze_GLFWwindow;
#else
typedef void ze_texture_t;

typedef void ze_cJSON;
typedef void ze_GLFWwindow;
#endif

#if defined(_ZADRAPANIE)
struct ze_texture {
	GLuint id;
};
#endif

struct ze_runtime {
	ze_cJSON*      json;
	ze_GLFWwindow* window;

	ze_target_t** targets;
	ze_thread_t** threads;
	ze_sprite_t** sprites;

	void*	       user;
	ze_load_file_t load_file;
	ze_bool	       turbo;
};

struct ze_target {
	ze_cJSON* json;

	ze_bool stage;

	ze_costume_t** costumes;
	ze_blockkv_t*  blocks;
	ze_block_t**   tree;
};

struct ze_costume {
	ze_cJSON* json;

	unsigned char* data;

	int	       width;
	int	       height;
	int	       rgba_width;
	int	       rgba_height;
	unsigned char* rgba;

	double center_x;
	double center_y;
	double resolution;

	ze_texture_t* texture;
};

struct ze_block {
	ze_cJSON* json;

	ze_block_t* parent;
	ze_block_t* children;
	ze_block_t* prev;
	ze_block_t* next;

	ze_inputkv_t* inputs;

	ze_bool toplevel;
	char*	opcode;
};

struct ze_blockkv {
	char*	    key;
	ze_block_t* value;
};

struct ze_thread {
	ze_sprite_t*	 sprite;
	ze_block_t*	 block;
	ze_block_t**	 stack;
	ze_check_loop_t* checkstack;

	ze_bool vsync;
	ze_bool stopped;
};

union ze_input {
	double number;
	char   color[8];
	char*  string;
	char*  broadcast;
	char*  variable;
	char*  list;
};

struct ze_inputkv {
	char*	   key;
	ze_input_t value;
	int	   type;
};

struct ze_sprite {
	ze_target_t* target;

	int costume;

	double x;
	double y;
	double angle;
};

/* runtime.c */
ZEDECL void ze_runtime_init(ze_runtime_t* rt);
ZEDECL void ze_runtime_load_project(ze_runtime_t* rt, const char* data, int size);
ZEDECL void ze_runtime_loop(ze_runtime_t* rt);
ZEDECL void ze_runtime_uninit(ze_runtime_t* rt);

/* render.c */
ZEDECL void ze_render(ze_runtime_t* rt);

/* log.c */
ZEDECL void ze_log(const char* fmt, ...);

/* string.c */
ZEDECL char* ze_string_dup(const char* str);

/* thread.c */
ZEDECL ze_thread_t* ze_thread_start(ze_runtime_t* rt, ze_block_t* block);
ZEDECL void	    ze_thread_stop(ze_thread_t* thread);
ZEDECL void	    ze_thread_exec(ze_thread_t* thread);
ZEDECL void	    ze_thread_kill(ze_runtime_t* rt, ze_thread_t* thread); /* you want to use ze_thread_stop - ze_thread_kill actually removes entry */

/* texture.c */
ZEDECL ze_texture_t* ze_texture_load(ze_costume_t* costume);
ZEDECL void	     ze_texture_free(ze_texture_t* texture);

/* target.c */
ZEDECL ze_target_t* ze_target_parse(ze_runtime_t* rt, ze_cJSON* json);
ZEDECL void	    ze_target_free(ze_target_t* target);

/* costume.c */
ZEDECL ze_costume_t* ze_costume_parse(ze_runtime_t* rt, ze_cJSON* json);
ZEDECL void	     ze_costume_free(ze_costume_t* costume);

/* block.c */
ZEDECL ze_block_t* ze_block_parse(ze_runtime_t* rt, ze_cJSON* json);
ZEDECL void	   ze_block_print(ze_block_t* block);
ZEDECL void	   ze_block_free(ze_block_t* block); /* this DOES NOT free tree */

/* sprite.c */
ZEDECL ze_sprite_t* ze_sprite_start(ze_runtime_t* rt, ze_target_t* target, ze_bool clone);
ZEDECL void	    ze_sprite_kill(ze_runtime_t* rt, ze_sprite_t* sprite); /* internal */

#endif
