#ifndef __BA_RUNTIME_H__
#define __BA_RUNTIME_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define BA_WIDTH 480
#define BA_HEIGHT 360

#define BA_FORMAT_MAX_DIGITS "16"
#define BA_FORMAT_DOUBLE "%." BA_FORMAT_MAX_DIGITS "lf"
#define BA_FORMAT_FLOAT "%." BA_FORMAT_MAX_DIGITS "f"

#if defined(_BENZYNA)
#include <cJSON.h>
#include <miniaudio.h>
#include <stb_ds.h>
#include <stb_image.h>
#include <nanosvg.h>
#include <nanosvgrast.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#if defined(_BENZYNA) && defined(_WIN32)
#define BADECL extern __declspec(dllexport)
#elif defined(_WIN32)
#define BADECL extern __declspec(dllimport)
#else
#define BADECL extern
#endif

typedef unsigned char ba_bool;

#define ba_false ((ba_bool)0)
#define ba_true ((ba_bool)1)

typedef struct ba_runtime      ba_runtime_t;
typedef struct ba_target       ba_target_t;
typedef struct ba_costume      ba_costume_t;
typedef struct ba_block	       ba_block_t;
typedef struct ba_blockkv      ba_blockkv_t;
typedef struct ba_thread       ba_thread_t;
typedef union ba_input_union   ba_input_union_t;
typedef struct ba_input	       ba_input_t;
typedef struct ba_inputkv      ba_inputkv_t;
typedef struct ba_sprite       ba_sprite_t;
typedef struct ba_stringkv     ba_stringkv_t;
typedef struct ba_stringlistkv ba_stringlistkv_t;

typedef unsigned char* (*ba_load_file_t)(ba_runtime_t* rt, const char* path, int* size);
typedef ba_bool (*ba_check_loop_t)(ba_thread_t* thread);

enum ba_input_type {
	ba_input_number = 0, /* treat 4/5/6/7/8 as same thing */
	ba_input_color,
	ba_input_string,
	ba_input_broadcast,
	ba_input_variable,
	ba_input_list,
	ba_input_block
};

enum ba_status {
	ba_status_ok = 0,
	ba_status_declined,
	ba_status_next
};

#if defined(_BENZYNA)
typedef struct ba_texture ba_texture_t;

typedef cJSON	   ba_cJSON;
typedef GLFWwindow ba_GLFWwindow;
#else
typedef void ba_texture_t;

typedef void ba_cJSON;
typedef void ba_GLFWwindow;
#endif

#if defined(_BENZYNA)
struct ba_texture {
	GLuint id;
};
#endif

struct ba_runtime {
	ba_cJSON*      json;
	ba_GLFWwindow* window;

	ba_target_t** targets;
	ba_thread_t** threads;
	ba_sprite_t** sprites;

	void*	       user;
	ba_load_file_t load_file;
	ba_bool	       turbo;
};

struct ba_target {
	ba_cJSON* json;

	ba_bool stage;

	ba_costume_t**	   costumes;
	ba_stringkv_t*	   variables;
	ba_stringlistkv_t* lists;
	ba_blockkv_t*	   blocks;
	ba_block_t**	   tree;
};

struct ba_costume {
	ba_cJSON* json;

	unsigned char* data;

	int	       width;
	int	       height;
	int	       rgba_width;
	int	       rgba_height;
	unsigned char* rgba;

	double center_x;
	double center_y;
	double resolution;

	ba_texture_t* texture;
};

struct ba_block {
	ba_cJSON* json;

	ba_block_t* parent;
	ba_block_t* children;
	ba_block_t* prev;
	ba_block_t* next;

	ba_inputkv_t* inputs;

	ba_bool toplevel;
	char*	opcode;
};

struct ba_blockkv {
	char*	    key;
	ba_block_t* value;
};

struct ba_thread {
	ba_runtime_t* runtime;

	ba_sprite_t*	 sprite;
	ba_block_t*	 block;
	ba_block_t**	 stack;
	ba_check_loop_t* checkstack;

	ba_bool vsync;
	ba_bool stopped;
};

union ba_input_union {
	double number;
	char   color[8];
	char*  string;
	char*  broadcast;
	char*  variable;
	char*  list;
	char*  block;
};

struct ba_input {
	int		 type;
	ba_input_union_t u;
};

struct ba_inputkv {
	char*	   key;
	ba_input_t value;
};

struct ba_sprite {
	ba_target_t* target;

	int costume;

	double x;
	double y;
	double angle;

	ba_stringkv_t* variables;
};

struct ba_stringkv {
	char* key;
	char* value;
};

struct ba_stringlistkv {
	char*  key;
	char** value;
};

/* runtime.c */
BADECL void	    ba_runtime_init(ba_runtime_t* rt);
BADECL void	    ba_runtime_load_project(ba_runtime_t* rt, const char* data, int size);
BADECL void	    ba_runtime_loop(ba_runtime_t* rt);
BADECL void	    ba_runtime_uninit(ba_runtime_t* rt);
BADECL ba_sprite_t* ba_runtime_get_stage_sprite(ba_runtime_t* rt);

/* render.c */
BADECL void ba_render(ba_runtime_t* rt);

/* log.c */
BADECL void ba_log(const char* fmt, ...);

/* string.c */
BADECL char* ba_string_dup(const char* str);
BADECL char* ba_string_concat(const char* str, ...);

/* thread.c */
BADECL ba_thread_t* ba_thread_start(ba_runtime_t* rt, ba_block_t* block);
BADECL void	    ba_thread_stop(ba_thread_t* thread);
BADECL void	    ba_thread_exec(ba_thread_t* thread);
BADECL void	    ba_thread_kill(ba_runtime_t* rt, ba_thread_t* thread); /* you want to use ba_thread_stop - ba_thread_kill actually removes entry */

/* texture.c */
BADECL ba_texture_t* ba_texture_load(ba_costume_t* costume);
BADECL void	     ba_texture_free(ba_texture_t* texture);

/* target.c */
BADECL ba_target_t* ba_target_parse(ba_runtime_t* rt, ba_cJSON* json);
BADECL void	    ba_target_free(ba_target_t* target);

/* costume.c */
BADECL ba_costume_t* ba_costume_parse(ba_runtime_t* rt, ba_cJSON* json);
BADECL void	     ba_costume_free(ba_costume_t* costume);

/* block.c */
BADECL ba_block_t* ba_block_parse(ba_runtime_t* rt, ba_cJSON* json);
BADECL void	   ba_block_print(ba_block_t* block);
BADECL void	   ba_block_free(ba_block_t* block); /* this DOES NOT free tree */

/* sprite.c */
BADECL ba_sprite_t* ba_sprite_start(ba_runtime_t* rt, ba_target_t* target);
BADECL void	    ba_sprite_kill(ba_runtime_t* rt, ba_sprite_t* sprite); /* internal */

/* exec.c */
BADECL char* ba_exec(ba_thread_t* thread, ba_input_t* value);

/* blocks */
BADECL int ba_block_control(ba_thread_t* thread);
BADECL int ba_block_looks(ba_thread_t* thread);

/* shadows */
BADECL char* ba_shadow_operator(ba_thread_t* thread, const char* block);

#endif
