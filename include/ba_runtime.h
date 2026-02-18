#ifndef __BA_RUNTIME_H__
#define __BA_RUNTIME_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#if !defined(_WIN32)
#include <unistd.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define BA_WIDTH 480
#define BA_HEIGHT 360

#define BA_FORMAT_MAX_DIGITS "15"
#define BA_FORMAT_DOUBLE "%." BA_FORMAT_MAX_DIGITS "lf"
#define BA_FORMAT_FLOAT "%." BA_FORMAT_MAX_DIGITS "f"

#if defined(_BENZYNA)
#include <cJSON.h>
#include <miniaudio.h>
#include <stb_ds.h>
#include <stb_image.h>
#include <nanosvg.h>
#include <nanosvgrast.h>
#include <zip.h>
#include <glad/glad.h>
#include <speech.h>
#include <dr_mp3.h>
#include <dr_wav.h>
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
#define ba_audio_rate 48000

typedef struct ba_runtime_param	   ba_runtime_param_t;
typedef struct ba_runtime	   ba_runtime_t;
typedef struct ba_target	   ba_target_t;
typedef struct ba_costume	   ba_costume_t;
typedef struct ba_block		   ba_block_t;
typedef struct ba_blockkv	   ba_blockkv_t;
typedef struct ba_thread	   ba_thread_t;
typedef union ba_input_union	   ba_input_union_t;
typedef struct ba_input		   ba_input_t;
typedef struct ba_inputkv	   ba_inputkv_t;
typedef struct ba_sprite	   ba_sprite_t;
typedef struct ba_stringkv	   ba_stringkv_t;
typedef struct ba_stringlistkv	   ba_stringlistkv_t;
typedef struct ba_block_handlerkv  ba_block_handlerkv_t;
typedef struct ba_shadow_handlerkv ba_shadow_handlerkv_t;
typedef struct ba_thread_stack	   ba_thread_stack_t;
typedef struct ba_thread_wait	   ba_thread_wait_t;
typedef union ba_runtime_union	   ba_runtime_union_t;
typedef struct ba_audio_stream	   ba_audio_stream_t;

typedef unsigned char* (*ba_load_file_t)(ba_runtime_t* rt, const char* path, int* size);
typedef void (*ba_swap_buffer_t)(ba_runtime_t* rt);
typedef void (*ba_make_current_t)(ba_runtime_t* rt);
typedef void (*ba_swap_interval_t)(ba_runtime_t* rt, int interval);
typedef ba_bool (*ba_thread_check_t)(ba_thread_t* thread);
typedef int (*ba_thread_block_handler_t)(ba_thread_t* thread);
typedef char* (*ba_thread_shadow_handler_t)(ba_thread_t* thread, ba_block_t* block);
typedef void (*ba_free_t)(void* arg);
typedef void (*ba_audio_stream_free_t)(ba_audio_stream_t* stream);
typedef int (*ba_audio_stream_read_t)(ba_audio_stream_t* stream, short* buffer, int wanted);

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
	ba_status_stay = 0,
	ba_status_next
};

#if defined(_BENZYNA)
typedef struct ba_texture		 ba_texture_t;
typedef struct ba_audio			 ba_audio_t;
typedef struct ba_audio_stream_processor ba_audio_stream_processor_t;

typedef cJSON	     ba_cJSON;
typedef struct zip_t ba_zip_t;
#else
typedef void ba_texture_t;
typedef void ba_audio_t;
typedef void ba_audio_stream_processor_t;

typedef void ba_cJSON;
typedef void ba_zip_t;
#endif

#if defined(_BENZYNA)
struct ba_texture {
	GLuint id;
};

struct ba_audio {
	ba_runtime_t* runtime;

	ba_bool init;

	ma_device	 device;
	ma_device_config config;
	ma_mutex	 mutex;

	ba_audio_stream_t** streams;
};

struct ba_audio_stream_processor {
	ma_resampler_config resampler_config;
	ma_resampler	    resampler;

	ma_channel_converter_config converter_config;
	ma_channel_converter	    converter;
};
#endif

struct ba_audio_stream {
	ba_audio_t* audio;

	ba_audio_stream_free_t free;
	ba_audio_stream_read_t read;

	void* opaque1;
	void* opaque2;

	int rate;
	int channel;

	ba_bool paused;
	ba_bool autoclean;

	ba_audio_stream_processor_t* processor;
};

struct ba_runtime_param {
	ba_load_file_t	   load_file;
	ba_swap_interval_t swap_interval;
	ba_make_current_t  make_current;
	ba_swap_buffer_t   swap_buffer;
	ba_bool		   turbo;
};

union ba_runtime_union {
	const char* root_path;
	ba_zip_t*   zip;
};

struct ba_runtime {
	ba_cJSON* json;

	ba_target_t** targets;
	ba_thread_t** threads;
	ba_sprite_t** sprites;

	ba_audio_t* audio;

	void* user;

	ba_runtime_param_t param;

	ba_block_handlerkv_t*  block_handlers;
	ba_shadow_handlerkv_t* shadow_handlers;

	ba_runtime_union_t u;
};

struct ba_target {
	ba_cJSON* json;

	ba_bool stage;

	ba_costume_t**	   costumes;
	ba_stringkv_t*	   variables; /* don't modify this! modify ba_sprite.variables instead. */
	ba_stringlistkv_t* lists;
	ba_blockkv_t*	   blocks;
	ba_block_t**	   tree;
	ba_stringkv_t*	   sounds;
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

	ba_inputkv_t*  inputs;
	ba_stringkv_t* fields;

	ba_bool toplevel;
	char*	opcode;
};

struct ba_blockkv {
	char*	    key;
	ba_block_t* value;
};

struct ba_thread_stack {
	ba_block_t*	  loop;	  /* where to jump to, if loop */
	ba_block_t*	  escape; /* where to escape to, if not loop */
	ba_thread_check_t check;  /* if function is non NULL and returns true, go loop */
	void*		  arg;
	ba_free_t	  free_arg;
};

struct ba_thread_wait {
	ba_thread_check_t check;
	void*		  arg;
	ba_free_t	  free_arg;
};

struct ba_thread {
	ba_runtime_t* runtime;

	ba_sprite_t* sprite;
	ba_block_t*  block;

	ba_thread_stack_t* stack;

	ba_bool vsync;
	ba_bool stopped;
	ba_bool autoclean;

	ba_thread_wait_t wait;
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

struct ba_block_handlerkv {
	char*			  key;
	ba_thread_block_handler_t value;
};

struct ba_shadow_handlerkv {
	char*			   key;
	ba_thread_shadow_handler_t value;
};

/* runtime.c */
BADECL ba_bool	    ba_runtime_init(ba_runtime_t* rt);
BADECL ba_bool	    ba_runtime_load_project(ba_runtime_t* rt, const char* data, int size);
BADECL void	    ba_runtime_step(ba_runtime_t* rt);
BADECL void	    ba_runtime_uninit(ba_runtime_t* rt);
BADECL ba_sprite_t* ba_runtime_get_stage_sprite(ba_runtime_t* rt);
BADECL ba_bool	    ba_runtime_load_path(ba_runtime_t* rt, const char* path);
BADECL void	    ba_runtime_block_handler(ba_runtime_t* rt, const char* name, ba_thread_block_handler_t handler);
BADECL void	    ba_runtime_shadow_handler(ba_runtime_t* rt, const char* name, ba_thread_shadow_handler_t handler);

/* audio.c */
BADECL ba_audio_t*	  ba_audio_open(ba_runtime_t* rt);
BADECL void		  ba_audio_close(ba_audio_t* audio);
BADECL ba_audio_stream_t* ba_audio_stream_new(ba_audio_t* audio);
BADECL void		  ba_audio_lock(ba_audio_t* audio);
BADECL void		  ba_audio_unlock(ba_audio_t* audio);
BADECL void		  ba_audio_stream_free(ba_audio_stream_t* stream);
BADECL void		  ba_audio_stream_set_paused(ba_audio_stream_t* stream, ba_bool paused);
BADECL ba_bool		  ba_audio_stream_get_paused(ba_audio_stream_t* stream);
BADECL void		  ba_audio_stream_set_autoclean(ba_audio_stream_t* stream, ba_bool autoclean);
BADECL void		  ba_audio_stream_set_rate(ba_audio_stream_t* stream, int rate);
BADECL void		  ba_audio_stream_set_channel(ba_audio_stream_t* stream, int channel);
BADECL void		  ba_audio_stream_init(ba_audio_stream_t* stream);

/* audio_file.c */
BADECL ba_audio_stream_t* ba_audio_file_open(ba_audio_t* audio, const char* path);

/* render.c */
BADECL void ba_render(ba_runtime_t* rt);

/* log.c */
BADECL void ba_log(const char* fmt, ...);

/* string.c */
BADECL char*   ba_string_dup(const char* str);
BADECL char*   ba_string_concat(const char* str, ...);
BADECL ba_bool ba_string_is_number(const char* str);
BADECL ba_bool ba_string_is_false(const char* str); /* otherwise - treat it as true */

/* thread.c */
BADECL ba_thread_t* ba_thread_start(ba_runtime_t* rt, ba_block_t* block);
BADECL void	    ba_thread_stop(ba_thread_t* thread);
BADECL void	    ba_thread_step(ba_thread_t* thread);
BADECL void	    ba_thread_kill(ba_runtime_t* rt, ba_thread_t* thread); /* you want to use ba_thread_stop - ba_thread_kill actually removes entry */
/* note - all of them actually allocate memory */
BADECL char* ba_thread_input(ba_thread_t* thread, const char* input); /* this gets input from current block ; useful for blocks */
BADECL char* ba_thread_input2(ba_thread_t* thread, ba_block_t* block, const char* input);
BADECL char* ba_thread_field(ba_thread_t* thread, const char* field); /* this gets field from current block ; useful for blocks */
BADECL char* ba_thread_field2(ba_thread_t* thread, ba_block_t* block, const char* field);

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
BADECL ba_input_t* ba_block_input(ba_block_t* block, const char* input);

/* sprite.c */
BADECL ba_sprite_t* ba_sprite_start(ba_runtime_t* rt, ba_target_t* target);
BADECL void	    ba_sprite_kill(ba_runtime_t* rt, ba_sprite_t* sprite); /* internal */

/* exec.c */
BADECL char* ba_exec(ba_thread_t* thread, ba_input_t* value);

/* time.c */
BADECL double ba_time_tick(void);

/* blocks */
BADECL void ba_block_motion(ba_runtime_t* rt);
BADECL void ba_block_looks(ba_runtime_t* rt);
BADECL void ba_block_sound(ba_runtime_t* rt);
BADECL void ba_block_control(ba_runtime_t* rt);
BADECL void ba_block_text2speech(ba_runtime_t* rt);

/* shadows */
BADECL void ba_shadow_sound(ba_runtime_t* rt);
BADECL void ba_shadow_operator(ba_runtime_t* rt);

#endif
