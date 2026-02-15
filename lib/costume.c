#include <ze_runtime.h>

ze_costume_t* ze_costume_parse(ze_runtime_t* rt, ze_cJSON* json) {
	ze_costume_t*  costume = malloc(sizeof(*costume));
	cJSON*	       md5ext;
	cJSON*	       dataFormat;
	unsigned char* data;
	int	       size;

	memset(costume, 0, sizeof(*costume));

	costume->json = json;

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
		ze_costume_free(costume);
		return NULL;
	}

	if(rt->load_file == NULL) {
		ze_costume_free(costume);
		return NULL;
	}

	if((data = rt->load_file(rt, md5ext->valuestring, &size)) == NULL) {
		ze_costume_free(costume);
		return NULL;
	}

	costume->data = malloc(size + 1);
	memcpy(costume->data, data, size);
	costume->data[size] = 0;

	if(strcmp(dataFormat->valuestring, "svg") == 0) {
		NSVGimage*	img = nsvgParse(costume->data, "px", 128);
		NSVGrasterizer* rast;
		double		n = (ZE_WIDTH > ZE_HEIGHT ? ZE_WIDTH : ZE_HEIGHT) * 2;
		double		s = 0;

		if(img == NULL) {
			ze_costume_free(costume);
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

		ze_log("%s: Vector image", md5ext->valuestring);
	} else {
		int ch;

		ze_log("%s: Bitmap image", md5ext->valuestring);

		if((costume->rgba = stbi_load_from_memory(costume->data, size, &costume->width, &costume->height, &ch, 4)) == NULL) {
			ze_costume_free(costume);
			return NULL;
		}

		costume->rgba_width  = costume->width;
		costume->rgba_height = costume->height;
	}
	free(data);

skip:;
	costume->texture = ze_texture_load(costume);

	return costume;
}

void ze_costume_free(ze_costume_t* costume) {
	if(costume->texture != NULL) ze_texture_free(costume->texture);
	if(costume->rgba != NULL) free(costume->rgba);
	if(costume->data != NULL) free(costume->data);
	free(costume);
}
