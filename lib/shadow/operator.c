#include <ba_runtime.h>

char* ba_shadow_operator(ba_thread_t* thread, const char* block) {
	ba_block_t* b	= shget(thread->sprite->target->blocks, block);
	char*	    str = NULL;

	if(b == NULL) return NULL;

	if(strcmp(b->opcode, "operator_round") == 0) {
		const char* nums = "NUM";
		int	    numi = shgeti(b->inputs, nums);
		char*	    numr;

		if(numi == -1) return NULL;

		numr = ba_exec(thread, &b->inputs[numi].value);

		if(numr) {
			double num = atof(numr);
			double n   = 0;

			if(strcmp(b->opcode, "operator_round") == 0) {
				n = round(num);
			}

			str = malloc(64);
			sprintf(str, BA_FORMAT_DOUBLE, n);
		}

		if(numr != NULL) free(numr);
	} else if(strcmp(b->opcode, "operator_add") == 0 || strcmp(b->opcode, "operator_subtract") == 0 || strcmp(b->opcode, "operator_multiply") == 0 || strcmp(b->opcode, "operator_divide") == 0 || strcmp(b->opcode, "operator_mod") == 0 || strcmp(b->opcode, "operator_random") == 0) {
		const char* num1s = (strcmp(b->opcode, "operator_random") == 0) ? "FROM" : "NUM1";
		const char* num2s = (strcmp(b->opcode, "operator_random") == 0) ? "TO" : "NUM2";
		int	    num1i = shgeti(b->inputs, num1s);
		int	    num2i = shgeti(b->inputs, num2s);
		char*	    num1r;
		char*	    num2r;

		if(num1i == -1 || num2i == -1) return NULL;

		num1r = ba_exec(thread, &b->inputs[num1i].value);
		num2r = ba_exec(thread, &b->inputs[num2i].value);

		if(num1r != NULL && num2r != NULL) {
			double num1 = atof(num1r);
			double num2 = atof(num2r);
			double n    = 0;

			if(strcmp(b->opcode, "operator_add") == 0) {
				n = num1 + num2;
			} else if(strcmp(b->opcode, "operator_subtract") == 0) {
				n = num1 - num2;
			} else if(strcmp(b->opcode, "operator_multiply") == 0) {
				n = num1 * num2;
			} else if(strcmp(b->opcode, "operator_divide") == 0) {
				n = num1 / num2;
			} else if(strcmp(b->opcode, "operator_mod") == 0) {
				n = num1 - num2 * (int)(num1 / num2);
			} else if(strcmp(b->opcode, "operator_random") == 0) {
				n = (rand() % (int)(num2 - num1)) + num1;
			}

			str = malloc(64);
			sprintf(str, BA_FORMAT_DOUBLE, n);
		}

		if(num1r != NULL) free(num1r);
		if(num2r != NULL) free(num2r);
	}

	return str;
}
