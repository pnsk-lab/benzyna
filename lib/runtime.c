#include <ba_runtime.h>

static ba_bool first = ba_true;

ba_bool ba_runtime_init(ba_runtime_t* rt) {
	ba_runtime_param_t param = rt->param;

	memset(rt, 0, sizeof(*rt));

	rt->param = param;

	sh_new_strdup(rt->block_handlers);
	shdefault(rt->block_handlers, NULL);

	sh_new_strdup(rt->shadow_handlers);
	shdefault(rt->shadow_handlers, NULL);

	ba_block_motion(rt);
	ba_block_looks(rt);
	ba_block_sound(rt);
	ba_block_control(rt);

	ba_block_text2speech(rt);

	ba_shadow_sound(rt);
	ba_shadow_operator(rt);

	if((rt->audio = ba_audio_open(rt)) == NULL) {
		ba_runtime_uninit(rt);

		return ba_false;
	}

	if(rt->param.make_current != NULL) rt->param.make_current(rt);

	if(first) {
		gladLoadGL();

		first = ba_false;
	}

	if(rt->param.swap_interval != NULL) rt->param.swap_interval(rt, rt->param.turbo ? 0 : 1);

	glEnable(GL_BLEND);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-BA_WIDTH / 2, BA_WIDTH / 2, -BA_HEIGHT / 2, BA_HEIGHT / 2, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return ba_true;
}

ba_bool ba_runtime_load_project(ba_runtime_t* rt, const char* data, int size) {
	cJSON* targets;
	int    i;

	if((rt->json = cJSON_ParseWithLength(data, size)) == NULL) return ba_false;

	if((targets = cJSON_GetObjectItem(rt->json, "targets")) == NULL || targets->type != cJSON_Array) return ba_false;

	ba_log("%d target(s)", cJSON_GetArraySize(targets));

	targets = targets->child;
	while(targets != NULL) {
		ba_target_t* t = ba_target_parse(rt, targets);
		if(t == NULL) {
			return ba_false;
		}

		arrput(rt->targets, t);

		targets = targets->next;
	}

	for(i = 0; i < arrlen(rt->targets); i++) {
		int	     j;
		ba_sprite_t* spr;
		cJSON*	     js;

		spr = ba_sprite_start(rt, rt->targets[i]);

		spr->angle = 90;

		js = cJSON_GetObjectItem(rt->targets[i]->json, "currentCostume");
		if(js != NULL && js->type == cJSON_Number) spr->costume = js->valuedouble;
		js = cJSON_GetObjectItem(rt->targets[i]->json, "x");
		if(js != NULL && js->type == cJSON_Number) spr->x = js->valuedouble;
		js = cJSON_GetObjectItem(rt->targets[i]->json, "y");
		if(js != NULL && js->type == cJSON_Number) spr->y = js->valuedouble;
		js = cJSON_GetObjectItem(rt->targets[i]->json, "direction");
		if(js != NULL && js->type == cJSON_Number) {
			spr->angle = js->valuedouble;
		}

		for(j = 0; j < arrlen(rt->targets[i]->tree); j++) {
			if(strcmp(rt->targets[i]->tree[j]->opcode, "event_whenflagclicked") == 0) {
				ba_thread_t* thread = ba_thread_start(rt, rt->targets[i]->tree[j]);

				thread->sprite	  = spr;
				thread->autoclean = ba_true; /* since we don't have to check their state, mark them as auto-cleanable */
			}
		}
	}

	return ba_true;
}

void ba_runtime_step(ba_runtime_t* rt) {
	int	i;
	ba_bool loop;

	do {
		loop = ba_false;
		for(i = 0; i < arrlen(rt->threads); i++) {
			if(rt->threads[i]->vsync || rt->threads[i]->stopped) continue;
			if(rt->threads[i]->wait.check != NULL && rt->threads[i]->wait.check(rt->threads[i])) continue;

			if(rt->threads[i]->wait.check != NULL) {
				if(rt->threads[i]->wait.arg != NULL) (rt->threads[i]->wait.free_arg != NULL ? rt->threads[i]->wait.free_arg : free)(rt->threads[i]->wait.arg);

				memset(&rt->threads[i]->wait, 0, sizeof(rt->threads[i]->wait));
			}

			loop = ba_true;

			ba_thread_step(rt->threads[i]);
		}
	} while(loop);

	for(i = 0; i < arrlen(rt->threads); i++) {
		rt->threads[i]->vsync = ba_false;
		if(rt->threads[i]->stopped && rt->threads[i]->autoclean) {
			ba_thread_kill(rt, rt->threads[i]);
			i--;
		}
	}

	ba_render(rt);
}

void ba_runtime_uninit(ba_runtime_t* rt) {
	int i;
	for(i = 0; i < arrlen(rt->threads); i++) {
		ba_thread_kill(rt, rt->threads[i]);
		i--;
	}
	arrfree(rt->threads);

	for(i = 0; i < arrlen(rt->sprites); i++) ba_sprite_kill(rt, rt->sprites[i]);
	arrfree(rt->sprites);

	for(i = 0; i < arrlen(rt->targets); i++) ba_target_free(rt->targets[i]);
	arrfree(rt->targets);

	shfree(rt->block_handlers);
	shfree(rt->shadow_handlers);

	if(rt->audio != NULL) ba_audio_close(rt->audio);

	if(rt->json != NULL) cJSON_Delete(rt->json);
}

ba_sprite_t* ba_runtime_get_stage_sprite(ba_runtime_t* rt) {
	int i;

	for(i = 0; i < arrlen(rt->sprites); i++) {
		if(rt->sprites[i]->target->stage) return rt->sprites[i];
	}

	return NULL;
}

static unsigned char* load_file_extracted(ba_runtime_t* rt, const char* path, int* size) {
	FILE*	       f;
	unsigned char* d;
	char*	       p = ba_string_concat(rt->u.root_path, "/", path, NULL);

	ba_log("Loading %s", p);

	if((f = fopen(p, "rb")) == NULL) {
		free(p);
		return NULL;
	}
	free(p);

	fseek(f, 0, SEEK_END);
	*size = ftell(f);
	fseek(f, 0, SEEK_SET);

	d = malloc(*size);
	fread(d, 1, *size, f);

	fclose(f);

	return d;
}

static unsigned char* load_file_zipped(ba_runtime_t* rt, const char* path, int* size) {
	unsigned char* d = NULL;
	int	       errnum;

	if((errnum = zip_entry_open(rt->u.zip, path)) != 0) {
		ba_log("Error accessing %s: %s", path, zip_strerror(errnum));
		return NULL;
	}

	ba_log("Accessing %s", path);

	/* docs recommend using zip_entry_extract which is 100% possible, but it uses a callback so more stuff has to be defined. refer to commit aeda2a if we want to do that. */
	if((errnum = zip_entry_read(rt->u.zip, (void**)&d, (size_t*)size)) < 0) {
		ba_log("Error reading %s: %s", path, zip_strerror(errnum));
		zip_entry_close(rt->u.zip);
		return NULL;
	};

	zip_entry_close(rt->u.zip);

	return d;
}

ba_bool ba_runtime_load_path(ba_runtime_t* rt, const char* path) {
	FILE*	    f;
	size_t	    sz = 0;
	char*	    buffer;
	struct stat s;
	ba_bool	    r = ba_true;

	stat(path, &s);

	if(access(path, F_OK) != 0) {
		ba_log("Failed to open %s: No such file or directory", path);
		return ba_false;
	}

	/* is path a directory? */
	if(S_ISREG(s.st_mode) == 0) {
		char* p;

		ba_log("Loading directory %s", path);

		p = ba_string_concat(path, "/project.json", NULL);
		if((f = fopen(p, "r")) == NULL) {
			ba_log("Failed to open %s: %s", path, strerror(errno));
			free(p);
			return ba_false;
		}
		free(p);

		fseek(f, 0, SEEK_END);
		sz = ftell(f);
		fseek(f, 0, SEEK_SET);

		buffer = malloc(sz);
		fread(buffer, 1, sz, f);

		fclose(f);

		rt->u.root_path	    = path;
		rt->param.load_file = load_file_extracted;

		r = ba_runtime_load_project(rt, buffer, sz);
		free(buffer);
	} else {
		char	fileinfo[15];
		ba_bool is_valid	  = ba_false;
		int	compression_level = 0;
		int	errnum		  = 0;

		if((f = fopen(path, "r")) == NULL) {
			ba_log("Failed to open file %s: %s", f, strerror(errno));
			return ba_false;
		}
		fread(fileinfo, 1, 15, f);
		fclose(f);

		/*
		 * Check if the file a valid zip file.
		 * (This is slightly overkill yes but who knows what people will try and I like having nice errors)
		 */
		if(fileinfo[0] == 0x50 || fileinfo[1] == 0x4b || fileinfo[2] == 0x03 || fileinfo[3] == 0x04) {
			is_valid = ba_true;
		}

		if(!is_valid) {
			ba_log("%s is not a valid .sb3 file. Please note that currently only .sb3 and extracted folders are supported.", f);
			return ba_false;
		}

		ba_log("Loading file %s", path);

		compression_level = (fileinfo[9] << 8) + fileinfo[8];

		ba_log("Compression level %d", compression_level);

		rt->u.zip = zip_openwitherror(path, compression_level, 'r', &errnum);
		if(errnum != 0) {
			ba_log("Error opening %s: %s", f, zip_strerror(errnum));
			return ba_false;
		}

		if((errnum = zip_entry_open(rt->u.zip, "project.json")) != 0) {
			ba_log("Error accessing %s/project.json: %s", f, zip_strerror(errnum));
			return ba_false;
		}

		sz = zip_entry_size(rt->u.zip);

		zip_entry_read(rt->u.zip, (void**)&buffer, &sz);
		zip_entry_close(rt->u.zip);

		rt->param.load_file = load_file_zipped;

		r = ba_runtime_load_project(rt, buffer, sz);
		free(buffer);
	}

	return r;
}

void ba_runtime_block_handler(ba_runtime_t* rt, const char* name, ba_thread_block_handler_t handler) {
	shput(rt->block_handlers, name, handler);
}

void ba_runtime_shadow_handler(ba_runtime_t* rt, const char* name, ba_thread_shadow_handler_t handler) {
	shput(rt->shadow_handlers, name, handler);
}
