#include <ze_runtime.h>

ze_sprite_t* ze_sprite_start(ze_runtime_t* rt, ze_target_t* target, ze_bool clone) {
	ze_sprite_t* sprite = malloc(sizeof(*sprite));

	memset(sprite, 0, sizeof(*sprite));

	sprite->target = target;

	sprite->x     = 0;
	sprite->y     = 0;
	sprite->angle = 0;

	arrput(rt->sprites, sprite);

	return sprite;
}

void ze_sprite_kill(ze_runtime_t* rt, ze_sprite_t* sprite) {
	int i;

	for(i = 0; i < arrlen(rt->sprites); i++) {
		if(rt->sprites[i] == sprite) {
			arrdel(rt->sprites, i);
			break;
		}
	}

	free(sprite);
}
