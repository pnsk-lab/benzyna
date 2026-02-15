#include <ba_runtime.h>

ba_costume_t* ba_costume_parse(ba_runtime_t* rt, ba_cJSON* json) {
	ba_costume_t*  costume = malloc(sizeof(*costume));
	cJSON*	       md5ext;
	cJSON*	       dataFormat;
	cJSON*	       coord;
	cJSON*	       res;
	unsigned char* data;
	int	       size;

	memset(costume, 0, sizeof(*costume));

	costume->json = json;

	if((res = cJSON_GetObjectItem(json, "bitmapResolution")) != NULL && res->type == cJSON_Number) {
		costume->resolution = res->valuedouble;
	}

	if((md5ext = cJSON_GetObjectItem(json, "md5ext")) == NULL || md5ext->type != cJSON_String) {
		costume->data	    = NULL;
		costume->rgba	    = malloc(4);
		costume->rgba_width = costume->width = 1;
		costume->rgba_height = costume->height = 1;

		memset(costume->rgba, 0, 4);

		/* TODO: get image from cdn */
		goto skip;
	}

	if((dataFormat = cJSON_GetObjectItem(json, "dataFormat")) == NULL || dataFormat->type != cJSON_String) {
		ba_costume_free(costume);
		return NULL;
	}

	if((coord = cJSON_GetObjectItem(json, "rotationCenterX")) != NULL && coord->type == cJSON_Number) {
		costume->center_x = coord->valuedouble;
	}

	if((coord = cJSON_GetObjectItem(json, "rotationCenterY")) != NULL && coord->type == cJSON_Number) {
		costume->center_y = coord->valuedouble;
	}

	if(rt->load_file == NULL) {
		ba_costume_free(costume);
		return NULL;
	}

	if((data = rt->load_file(rt, md5ext->valuestring, &size)) == NULL) {
		ba_costume_free(costume);
		return NULL;
	}

	costume->data = malloc(size + 1);
	memcpy(costume->data, data, size);
	costume->data[size] = 0;

	if(strcmp(dataFormat->valuestring, "svg") == 0) {
		NSVGimage*	img = nsvgParse(costume->data, "px", 128);
		NSVGrasterizer* rast;
		double		n = (BA_WIDTH > BA_HEIGHT ? BA_WIDTH : BA_HEIGHT) * 2;
		double		s = 0;

		if(img == NULL) {
			ba_costume_free(costume);
			return NULL;
		}

		rast = nsvgCreateRasterizer();

		s = (img->width < img->height) ? img->width : img->height;

		costume->width	     = img->width;
		costume->height	     = img->height;
		costume->rgba_width  = n / s * costume->width;
		costume->rgba_height = n / s * costume->height;
		costume->rgba	     = malloc(4 * costume->rgba_width * costume->rgba_height);

		nsvgRasterize(rast, img, 0, 0, n / s, costume->rgba, costume->rgba_width, costume->rgba_height, costume->rgba_width * 4);

		nsvgDeleteRasterizer(rast);
		nsvgDelete(img);

		ba_log("%s: Vector image", md5ext->valuestring);
	} else {
		int ch;

		ba_log("%s: Bitmap image", md5ext->valuestring);

		if((costume->rgba = stbi_load_from_memory(costume->data, size, &costume->width, &costume->height, &ch, 4)) == NULL) {
			ba_costume_free(costume);
			return NULL;
		}

		costume->rgba_width  = costume->width;
		costume->rgba_height = costume->height;
	}
	free(data);

skip:;
	costume->texture = ba_texture_load(costume);

	return costume;
}

void ba_costume_free(ba_costume_t* costume) {
	if(costume->texture != NULL) ba_texture_free(costume->texture);
	if(costume->rgba != NULL) free(costume->rgba);
	if(costume->data != NULL) free(costume->data);
	free(costume);
}
