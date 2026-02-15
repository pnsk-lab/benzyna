#include <ba_runtime.h>

int ba_block_motion(ba_thread_t* thread) {
	if(strcmp(thread->block->opcode, "motion_movesteps") == 0) {
		const char* s	= "STEPS";
		int	    ind = shgeti(thread->block->inputs, s);

		if(ind != -1) {
			char* str;

			if((str = ba_exec(thread, &thread->block->inputs[ind].value)) != NULL) {
				double c = cos(-thread->sprite->angle / 180 * M_PI);
				double s = sin(-thread->sprite->angle / 180 * M_PI);

				thread->sprite->x += c * atof(str);
				thread->sprite->y += s * atof(str);

				free(str);
			}
		}

		thread->vsync = ba_true;
	} else if(strcmp(thread->block->opcode, "motion_turnleft") == 0 || strcmp(thread->block->opcode, "motion_turnright") == 0) {
		const char* s	= "DEGREES";
		int	    ind = shgeti(thread->block->inputs, s);

		if(ind != -1) {
			char* str;

			if((str = ba_exec(thread, &thread->block->inputs[ind].value)) != NULL) {
				thread->sprite->angle += atof(str) * (strcmp(thread->block->opcode, "motion_turnleft") == 0 ? -1 : 1);
				free(str);
			}
		}
	} else if(strcmp(thread->block->opcode, "motion_pointindirection") == 0) {
		const char* s	= "DIRECTION";
		int	    ind = shgeti(thread->block->inputs, s);

		if(ind != -1) {
			char* str;

			if((str = ba_exec(thread, &thread->block->inputs[ind].value)) != NULL) {
				thread->sprite->angle += atof(str);
				free(str);
			}
		}
	} else if(strcmp(thread->block->opcode, "motion_gotoxy") == 0) {
		const char* s1 = "X";
		const char* s2 = "Y";
		int	    i1 = shgeti(thread->block->inputs, s1);
		int	    i2 = shgeti(thread->block->inputs, s2);

		if(i1 != -1 && i2 != -1) {
			char* str1;
			char* str2;

			if((str1 = ba_exec(thread, &thread->block->inputs[i1].value)) != NULL && (str2 = ba_exec(thread, &thread->block->inputs[i2].value)) != NULL) {
				thread->sprite->x = atof(str1);
				thread->sprite->y = atof(str2);
			}

			if(str1 != NULL) free(str1);
			if(str2 != NULL) free(str2);
		}
	} else {
		return ba_status_declined;
	}

	return ba_status_next;
}
