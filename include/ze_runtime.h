#ifndef __ZE_RUNTIME_H__
#define __ZE_RUNTIME_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

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

typedef unsigned char* (*ze_load_file_t)(ze_runtime_t* rt, const char* path, int* size);

#if defined(_ZADRAPANIE)
typedef cJSON	       ze_cJSON;
typedef NSVGimage      ze_NSVGimage;
typedef NSVGrasterizer ze_NSVGrasterizer;
typedef GLFWwindow     ze_GLFWwindow;
#else
typedef void ze_cJSON;
typedef void ze_NSVGimage;
typedef void ze_NSVGrasterizer;
typedef void ze_GLFWwindow;
#endif

struct ze_runtime {
	ze_cJSON*      root;
	ze_GLFWwindow* window;

	ze_target_t** targets;

	void*	       user;
	ze_load_file_t load_file;
};

struct ze_target {
	int costume;

	ze_costume_t** costumes;
};

struct ze_costume {
	unsigned char* data;

	ze_NSVGimage*	   svg_image;
	ze_NSVGrasterizer* svg_raster;

	int	       width;
	int	       height;
	unsigned char* rgba;
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

/* target.c */
ZEDECL ze_target_t* ze_target_parse(ze_runtime_t* rt, ze_cJSON* json);
ZEDECL void	    ze_target_free(ze_target_t* target);

/* costume.c */
ZEDECL ze_costume_t* ze_costume_parse(ze_runtime_t* rt, ze_cJSON* json);
ZEDECL void	     ze_costume_free(ze_costume_t* costume);

#endif
