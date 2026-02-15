#include <ba_runtime.h>

ba_texture_t* ba_texture_load(ba_costume_t* costume) {
	ba_texture_t* texture = malloc(sizeof(*texture));

	memset(texture, 0, sizeof(*texture));

	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, costume->rgba_width, costume->rgba_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, costume->rgba);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void ba_texture_free(ba_texture_t* texture) {
	glDeleteTextures(1, &texture->id);

	free(texture);
}
