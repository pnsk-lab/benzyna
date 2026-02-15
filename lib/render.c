#include <ze_runtime.h>

void ze_render(ze_runtime_t* rt) {
	int i;
	glfwMakeContextCurrent(rt->window);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	for(i = 0; i < arrlen(rt->sprites); i++) {
		ze_sprite_t* spr = rt->sprites[i];
		double	     cx	 = spr->target->costumes[spr->costume]->center_x;
		double	     cy	 = spr->target->costumes[spr->costume]->center_y;
		double	     w	 = spr->target->costumes[spr->costume]->width;
		double	     h	 = spr->target->costumes[spr->costume]->height;

		cx /= spr->target->costumes[spr->costume]->resolution;
		cy /= spr->target->costumes[spr->costume]->resolution;
		w /= spr->target->costumes[spr->costume]->resolution;
		h /= spr->target->costumes[spr->costume]->resolution;

		glBindTexture(GL_TEXTURE_2D, spr->target->costumes[spr->costume]->texture->id);
		glColor3f(1, 1, 1);
		glPushMatrix();
		glTranslatef(spr->x, spr->y, 0);
		glRotatef(-spr->angle, 0, 0, 1);
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
	glDisable(GL_TEXTURE_2D);

	glfwSwapBuffers(rt->window);
}
