#include <ba_runtime.h>

static void draw_sprite(ba_runtime_t* rt, ba_sprite_t* spr) {
	double cx = spr->target->costumes[spr->costume]->center_x;
	double cy = spr->target->costumes[spr->costume]->center_y;
	double w  = spr->target->costumes[spr->costume]->width;
	double h  = spr->target->costumes[spr->costume]->height;

	cx /= spr->target->costumes[spr->costume]->resolution;
	cy /= spr->target->costumes[spr->costume]->resolution;
	w /= spr->target->costumes[spr->costume]->resolution;
	h /= spr->target->costumes[spr->costume]->resolution;

	glBindTexture(GL_TEXTURE_2D, spr->target->costumes[spr->costume]->texture->id);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(spr->x, spr->y, 0);
	glRotatef(-spr->angle + 90, 0, 0, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-cx, cy);

	glTexCoord2f(0, 1);
	glVertex2f(-cx, cy - h);

	glTexCoord2f(1, 1);
	glVertex2f(-cx + w, cy - h);

	glTexCoord2f(1, 0);
	glVertex2f(-cx + w, cy);
	glEnd();
	glPopMatrix();
}

void ba_render(ba_runtime_t* rt) {
	int i;
	if(rt->param.make_current != NULL) rt->param.make_current(rt);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	/* draw stage */
	for(i = 0; i < arrlen(rt->sprites); i++) {
		if(rt->sprites[i]->target->stage) draw_sprite(rt, rt->sprites[i]);
	}
	glDisable(GL_TEXTURE_2D);

	for(i = 0; i < arrlen(rt->pens); i++) {
		double rgb[3];

		ba_hsv_to_rgb(rgb, rt->pens[i]->color);

		glColor3f(rgb[0], rgb[1], rgb[2]);

		if(arrlen(rt->pens[i]->coords) == 2) {
			glBegin(GL_POINT);
			glVertex2f(rt->pens[i]->coords[0], rt->pens[i]->coords[1]);
			glEnd();
		} else if(arrlen(rt->pens[i]->coords) > 2) {
			GLfloat* c = malloc(sizeof(*c) * arrlen(rt->pens[i]->coords));
			int	 j;

			for(j = 0; j < arrlen(rt->pens[i]->coords); j++) c[j] = rt->pens[i]->coords[j];

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, c);
			glDrawArrays(GL_LINE_STRIP, 0, arrlen(rt->pens[i]->coords) / 2);
			glDisableClientState(GL_VERTEX_ARRAY);

			free(c);
		}
	}

	glEnable(GL_TEXTURE_2D);
	/* draw sprites. */
	for(i = 0; i < arrlen(rt->sprites); i++) {
		if(!rt->sprites[i]->target->stage) draw_sprite(rt, rt->sprites[i]);
	}
	glDisable(GL_TEXTURE_2D);

	if(rt->param.swap_buffer != NULL) rt->param.swap_buffer(rt);
}
