#include <ze_runtime.h>

ze_costume_t* ze_costume_parse(ze_runtime_t* rt, ze_cJSON* json) {
	ze_costume_t*  costume = malloc(sizeof(*costume));
	cJSON*	       md5ext;
	cJSON*	       dataFormat;
	unsigned char* data;
	int	       size;

	memset(costume, 0, sizeof(*costume));

	if((md5ext = cJSON_GetObjectItem(json, "md5ext")) == NULL || md5ext->type != cJSON_String) {
		ze_costume_free(costume);
		return NULL;
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
		if((costume->svg_image = nsvgParse(costume->data, "px", 96)) == NULL) {
			ze_costume_free(costume);
			return NULL;
		}
		costume->svg_raster = nsvgCreateRasterizer();

		ze_log("%s: Vector image", md5ext->valuestring);
	} else {
		int ch;

		ze_log("%s: Bitmap image", md5ext->valuestring);

		if((costume->rgba = stbi_load_from_memory(costume->data, size, &costume->width, &costume->height, &ch, 4)) == NULL) {
			ze_costume_free(costume);
			return NULL;
		}
	}
	free(data);

	return costume;
}

void ze_costume_free(ze_costume_t* costume) {
	if(costume->rgba != NULL) free(costume->rgba);
	if(costume->svg_raster != NULL) nsvgDeleteRasterizer(costume->svg_raster);
	if(costume->svg_image != NULL) nsvgDelete(costume->svg_image);
	if(costume->data != NULL) free(costume->data);
	free(costume);
}
