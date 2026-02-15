#include <ba_runtime.h>

ba_sprite_t* ba_sprite_start(ba_runtime_t* rt, ba_target_t* target, ba_bool clone) {
	ba_sprite_t* sprite = malloc(sizeof(*sprite));

	memset(sprite, 0, sizeof(*sprite));

	sprite->target = target;

	sprite->x     = 0;
	sprite->y     = 0;
	sprite->angle = 0;

	arrput(rt->sprites, sprite);

	return sprite;
}

void ba_sprite_kill(ba_runtime_t* rt, ba_sprite_t* sprite) {
	int i;

	for(i = 0; i < arrlen(rt->sprites); i++) {
		if(rt->sprites[i] == sprite) {
			arrdel(rt->sprites, i);
			break;
		}
	}

	free(sprite);
}
