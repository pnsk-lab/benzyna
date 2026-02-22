#include <ba_runtime.h>

ba_sprite_t* ba_sprite_start(ba_runtime_t* rt, ba_target_t* target) {
	ba_sprite_t* sprite = malloc(sizeof(*sprite));
	int	     i;

	memset(sprite, 0, sizeof(*sprite));

	sprite->target = target;

	sprite->x     = 0;
	sprite->y     = 0;
	sprite->angle = 0;

	sprite->pen_color[0] = 0;
	sprite->pen_color[1] = 0;
	sprite->pen_color[2] = 1;

	arrput(rt->sprites, sprite);

	sh_new_strdup(sprite->variables);
	for(i = 0; i < shlen(target->variables); i++) {
		char* v = ba_string_dup(target->variables[i].value);

		shput(sprite->variables, target->variables[i].key, v);
	}

	return sprite;
}

void ba_sprite_kill(ba_runtime_t* rt, ba_sprite_t* sprite) {
	int i;

	for(i = 0; i < shlen(sprite->variables); i++) free(sprite->variables[i].value);
	shfree(sprite->variables);

	for(i = 0; i < arrlen(rt->sprites); i++) {
		if(rt->sprites[i] == sprite) {
			arrdel(rt->sprites, i);
			break;
		}
	}

	free(sprite);
}
